#include "Model.h"
#include "Comm/JsonDecoder.h"
#include "Comm/Usb.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/PrinterStatus.h"
#include "Presenter.h"
#include "View.h"
#include "lv_i18n/lv_i18n.h"
#include "lvgl/src/osal/lv_os.h"
#include "utils/StorageHelper.h"

Model::Model()
{
	// Timers
	m_timers.tick = lv_timer_create([](lv_timer_t* timer)
									{ static_cast<Model*>(lv_timer_get_user_data(timer))->post<EventType::Tick>(); },
									MODEL_TICK_INTERVAL,
									this);

#if !MULTITHREADED
	m_timers.request =
		lv_timer_create([](lv_timer_t* timer) { static_cast<Model*>(lv_timer_get_user_data(timer))->requestNewData(); },
						Comm::DUET.GetScaledPollInterval(),
						this);
	m_timers.receive = lv_timer_create(
		[](lv_timer_t* timer) { static_cast<Model*>(lv_timer_get_user_data(timer))->receiveNewUsbData(); }, 5, this);
#endif

	addEventListener<EventType::Connected>(this, &Model::connected);
	addEventListener<EventType::Disconnected>(this, &Model::disconnected);
}

void Model::bind(std::shared_ptr<UI::BasePresenter> presenter)
{
	UI_LOCK();
	unbind(presenter);
	m_presenters.push_back(presenter);
}

void Model::unbind(std::shared_ptr<UI::BasePresenter> presenter)
{
	UI_LOCK();
	LOG_DBG("Unbinding presenter {:s}", presenter->getName());
	m_presenters.remove(presenter);
}

void Model::startEventLoop()
{
	if (m_running)
	{
		LOG_WARN("Event loop already running");
		return;
	}
	m_running = true;
	m_eventThread = std::thread(&Model::runEventLoop, this);
}

void Model::stopEventLoop()
{
	if (!m_running)
	{
		LOG_WARN("Event loop not running");
		return;
	}
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_running = false;
	}
	m_eventCondition.notify_all();
	if (m_eventThread.joinable())
	{
		m_eventThread.join();
	}
}

void Model::runEventLoop()
{
	DeadlockDetector::getInstance().allowThreadToTakeMultipleLocks(Log::GetThreadId(), true);

	while (true)
	{
		std::pair<EventType, EventData> event;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_eventCondition.wait(lock, [this] { return !m_eventQueue.empty() || !m_running; });
			if (!m_running && m_eventQueue.empty())
			{
				LOG_DBG("Stopping event loop");
				break;
			}
			event = std::move(m_eventQueue.front());
			m_eventQueue.pop();
		}

		bool found = false;

		UI_LOCK();
		{
			auto it = m_handlers.find(event.first);
			if (it != m_handlers.end())
			{
				found = true;
				auto& handlers = it->second;
				for (auto& handler : handlers)
				{
					handler(event.second);
				}
			}
		}

		{
			auto it = m_presenters.begin();
			std::shared_ptr<UI::BasePresenter> presenter;
			while (it != m_presenters.end())
			{
				presenter = *it;
				if (!presenter->isActive())
				{
					++it;
					continue;
				}
				auto handler = presenter->getEventHandler(event.first);
				if (handler)
				{
					found = true;
					LOG_DBG("Notifying presenter {:s} for event {:d}", presenter->getName(), (int)event.first);

					handler(event.second);
				}
				++it;
			}
		}

		if (!found)
		{
			LOG_DBG("No handler for event type {:d}", (int)event.first);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

useconds_t Model::requestNewData()
{
	bool seqAvailable = Comm::sendNext();
#if 0
	if (seqAvailable && Comm::DUET.GetCommunicationType() == Comm::CommunicationType::network)
	{
		return 50 * 1000; // 50ms
	}
#endif
	return Comm::DUET.GetScaledPollInterval() * 1000; // Poll interval in microseconds
}

useconds_t Model::receiveNewUsbData()
{
	static constexpr useconds_t s_reconnectDelay = 1000 * 1000; // 1s
	static constexpr useconds_t s_pollInterval = 5 * 1000;		// 5ms
	static constexpr size_t bufferSize = 32768;
	static Comm::JsonDecoder decoder;
	static BYTE buffer[bufferSize];
	static size_t bufferLen = 0;

	if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::usb)
	{
		return s_reconnectDelay;
	}

	if (!Comm::getCurrentUsbDevice().isConnected())
	{
		LOG_VERBOSE("USB device disconnected");
		return s_reconnectDelay;
	}

	int len = Comm::getCurrentUsbDevice().receive(buffer + bufferLen, bufferSize - bufferLen);

	if (len > 0)
	{
		bufferLen += len;
		if (bufferLen >= bufferSize)
		{
			LOG_ERROR("Buffer overflow");
			bufferLen = 0;
			return s_pollInterval;
		}
	}
	else if (len < 0)
	{
		LOG_ERROR("Error receiving data");
		bufferLen = 0;
		memset(buffer, 0, bufferSize);
		return s_reconnectDelay;
	}

	if (bufferLen > 0 && buffer[bufferLen - 1] == '\n')
	{
		// Process the data
		LOG_DBG("Received {:d} bytes", bufferLen);
		decoder.CheckInput(buffer, bufferLen);
		bufferLen = 0;
		memset(buffer, 0, bufferSize);
	}
	return s_pollInterval;
}

void Model::runSubscribers(const char* key, Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	auto& subscribers = getSubscribers(key);
	if (subscribers.size() != 0)
	{
		LOG_DBG("found {:d} subscribers for '{:s}'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			subscriber.run(decoder, data, indices);
		}
	}
}

void Model::runArrayEndSubscribers(const char* key, Comm::JsonDecoder* decoder, const size_t indices[])
{
	auto& subscribers = getArrayEndSubscribers(key);
	if (subscribers.size() != 0)
	{
		LOG_DBG("found {:d} array end subscribers for '{:s}'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			subscriber.run(decoder, indices);
		}
	}
}

void Model::connected()
{
	LOG_DBG("Connected event");
	if (StorageHelper::getData(ID_DISPLAY_CONNECTED_MESSAGE, true))
	{
		post<EventType::Response>(_("connected_message"));
	}
}

void Model::disconnected()
{
	LOG_DBG("Disconnected event");
	if (StorageHelper::getData(ID_DISPLAY_CONNECTED_MESSAGE, true))
	{
		post<EventType::Response>(_("disconnected_message"));
	}
}

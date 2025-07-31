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

void Model::bind(std::weak_ptr<UI::BasePresenter> presenter)
{
	UI_LOCK();
	if (presenter.expired())
	{
		LOG_WARN("Attempted to bind an expired presenter");
		return;
	}
	unbind(presenter);
	LOG_DBG("Binding presenter '{:s}'", presenter.lock()->getName());
	m_presenters.push_back(presenter);
}

void Model::unbind(std::weak_ptr<UI::BasePresenter> presenter)
{
	UI_LOCK();
	if (presenter.expired())
	{
		LOG_WARN("Attempted to unbind an expired presenter");
		return;
	}
	std::shared_ptr<UI::BasePresenter> sharedPresenter = presenter.lock();
	m_presenters.remove_if(
		[&sharedPresenter](const std::weak_ptr<UI::BasePresenter>& p)
		{
			bool remove = false;
			if (p.expired())
			{
				LOG_DBG("Unbinding expired presenter");
				remove = true;
			}
			else if (p.lock() == sharedPresenter)
			{
				LOG_DBG("Unbinding presenter '{:s}'", sharedPresenter->getName());
				remove = true;
			}
			return remove;
		});
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
				presenter = (*it).lock();
				if (!presenter)
				{
					it = m_presenters.erase(it);
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
	static constexpr useconds_t s_pollInterval = 100 * 1000;	// 100ms
	static constexpr size_t s_bufferSize = 32768;
	static Comm::JsonDecoder s_decoder;
	static BYTE s_buffer[s_bufferSize];

	if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::usb)
	{
		return s_reconnectDelay;
	}

	if (!Comm::getCurrentUsbDevice().isConnected())
	{
		LOG_VERBOSE("USB device disconnected");
		return s_reconnectDelay;
	}

	int len = 0;
	auto err = Comm::getCurrentUsbDevice().receive(s_buffer, s_bufferSize, len);

	if (len < 0)
	{
		LOG_ERROR("This should be impossible, len < 0: {:d}", len);
		len = 0;
	}

	switch (err)
	{
	case Comm::UsbDevice::receive_err_t::NONE:
		s_decoder.CheckInput(s_buffer, len);
		break;
	case Comm::UsbDevice::receive_err_t::BUFFER_TOO_SMALL:
		LOG_ERROR("USB receive buffer too small");
		break;
	case Comm::UsbDevice::receive_err_t::TIMEOUT:
	case Comm::UsbDevice::receive_err_t::BUSY:
		break;
	case Comm::UsbDevice::receive_err_t::NO_DEVICE:
	case Comm::UsbDevice::receive_err_t::OTHER_ERROR:
		LOG_DBG("Resetting decoder");
		s_decoder.Reset();
		return s_reconnectDelay;
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
		post<EventType::Response>(std::string(_("connected_message")));
	}
}

void Model::disconnected()
{
	LOG_DBG("Disconnected event");
	if (StorageHelper::getData(ID_DISPLAY_CONNECTED_MESSAGE, true))
	{
		post<EventType::Response>(std::string(_("disconnected_message")));
	}
}

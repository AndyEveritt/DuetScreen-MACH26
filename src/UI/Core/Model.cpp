#include "Model.h"
#include "Comm/JsonDecoder.h"
#include "Comm/Usb.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/PrinterStatus.h"
#include "Presenter.h"
#include "View.h"
#include "i18n/i18n.h"
#include "lvgl/src/osal/lv_os.h"
#include "nameof.hpp"
#include "tracy/Tracy.hpp"
#include "utils/StorageHelper.h"

Model::Model()
{
	ZoneScoped;
	LOG_INFO("Initializing Model...");
	// Timers
	if (lv_is_initialized())
	{
		m_timers.tick = lv_timer_create(
			[](lv_timer_t* timer) { static_cast<Model*>(lv_timer_get_user_data(timer))->post<EventType::Tick>(); },
			MODEL_TICK_INTERVAL,
			this);
	}
	else
	{
		LOG_WARN("LVGL not initialized, skipping model tick timer creation");
		m_timers.tick = nullptr;
	}

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
	ZoneScoped;
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
	ZoneScoped;
	UI_LOCK();
	if (presenter.expired())
	{
		LOG_WARN("Attempted to unbind an expired presenter");
		return;
	}
	std::shared_ptr<UI::BasePresenter> sharedPresenter = presenter.lock();
	std::erase_if(m_presenters,
				  [&sharedPresenter](const std::weak_ptr<UI::BasePresenter>& p)
				  {
					  if (p.expired())
					  {
						  LOG_DBG("Unbinding expired presenter");
						  return true;
					  }
					  else if (p.lock() == sharedPresenter)
					  {
						  LOG_DBG("Unbinding presenter '{:s}'", sharedPresenter->getName());
						  return true;
					  }
					  return false;
				  });
}

void Model::startEventLoop()
{
	ZoneScoped;
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
	ZoneScoped;
	if (!m_running)
	{
		LOG_WARN("Event loop not running");
		return;
	}
	{
		std::lock_guard<LockableBase(std::mutex)> lock(m_mutex);
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
	tracy::SetThreadName("Model Event Loop");
	DeadlockDetector::getInstance().allowThreadToTakeMultipleLocks(Log::GetThreadId(), true);

	while (true)
	{
		{
			ZoneScoped;
			std::pair<EventType, EventData> event;
			{
				std::unique_lock<LockableBase(std::mutex)> lock(m_mutex);
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
				ZoneScopedN("Model Event Handlers");
				auto it = m_handlers.find(event.first);
				if (it != m_handlers.end())
				{
					found = true;
					auto& handlers = it->second;
					for (auto& handler : handlers)
					{
						ZoneScoped;
						[[maybe_unused]] const auto eventName = nameof::nameof_enum(event.first);
						ZoneName(event.first == EventType::Null ? "Null Event" : eventName.data(), eventName.size());
						ZoneColor(tracy::Color::Yellow);
						std::invoke(handler, event.second);
					}
				}
			}

			{
				ZoneScopedN("Presenter Event Handlers");
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
					ZoneScoped;
					ZoneName(presenter->getName().data(), presenter->getName().size());
					auto handler = presenter->getEventHandler(event.first);
					if (handler)
					{
						found = true;
						LOG_DBG("Notifying presenter '{:s}' for event '{:s}'",
								presenter->getName(),
								nameof::nameof_enum(event.first));

						ZoneScoped;
						[[maybe_unused]] const auto eventName = nameof::nameof_enum(event.first);
						ZoneName(event.first == EventType::Null ? "Null Event" : eventName.data(), eventName.size());
						ZoneColor(tracy::Color::Red);
						std::invoke(handler, event.second);
					}
					++it;
				}
			}

			if (!found)
			{
				LOG_VERBOSE("No handler for event type {:s}", nameof::nameof_enum(event.first));
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

std::chrono::milliseconds Model::requestNewData()
{
	ZoneScoped;
	bool seqAvailable = Comm::sendNext();
#if 0
	if (seqAvailable && Comm::DUET.GetCommunicationType() == Comm::CommunicationType::network)
	{
		return 50; // 50ms
	}
#else
	UNUSED(seqAvailable);
#endif
	return Comm::DUET.GetScaledPollInterval();
}

void Model::runSubscribers(const char* key, Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	auto& subscribers = getSubscribers(key);
	if (subscribers.size() != 0)
	{
		LOG_DBG("found {:d} subscribers for '{:s}'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			ZoneScopedN("Running subscriber");
			subscriber.run(decoder, data, indices);
		}
	}
}

void Model::runArrayEndSubscribers(const char* key, Comm::JsonDecoder* decoder, const size_t indices[])
{
	ZoneScoped;
	auto& subscribers = getArrayEndSubscribers(key);
	if (subscribers.size() != 0)
	{
		LOG_DBG("found {:d} array end subscribers for '{:s}'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			ZoneScopedN("Running subscriber");
			subscriber.run(decoder, indices);
		}
	}
}

void Model::connected()
{
	ZoneScoped;
	LOG_DBG("Connected event");
	if (StorageHelper::getData(ID_DISPLAY_CONNECTED_MESSAGE, true))
	{
		post<EventType::Response>(ResponseType::INFO, _("message.connected"));
	}
}

void Model::disconnected()
{
	ZoneScoped;
	LOG_DBG("Disconnected event");
	if (StorageHelper::getData(ID_DISPLAY_CONNECTED_MESSAGE, true))
	{
		post<EventType::Response>(ResponseType::INFO, _("message.disconnected"));
	}
}

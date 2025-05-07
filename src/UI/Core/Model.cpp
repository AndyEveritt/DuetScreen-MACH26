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

#define NOTIFY_ALL_PRESENTERS(func, ...)                                                                               \
  {                                                                                                                    \
	LOG_DBG("Notifying presenters for event: " #func);                                                                 \
	UI_LOCK();                                                                                                         \
	auto it = m_presenters.begin();                                                                                    \
	std::shared_ptr<UI::BasePresenter> presenter;                                                                      \
	while (true)                                                                                                       \
	{                                                                                                                  \
	  if (it == m_presenters.end())                                                                                    \
	  {                                                                                                                \
		break;                                                                                                         \
	  }                                                                                                                \
	  presenter = *it;                                                                                                 \
	  if (!presenter->isActive())                                                                                      \
	  {                                                                                                                \
		++it;                                                                                                          \
		continue;                                                                                                      \
	  }                                                                                                                \
	  LOG_DBG("Notifying presenter {:s}: " #func, presenter->getName());                                               \
	  presenter->func(__VA_ARGS__);                                                                                    \
	  ++it;                                                                                                            \
	}                                                                                                                  \
  }

#define MODEL_NOTIFICATION(func, ...)                                                                                  \
  void Model::func(__VA_ARGS__)                                                                                        \
  {                                                                                                                    \
	NOTIFY_ALL_PRESENTERS(func, __VA_ARGS__);                                                                          \
  }

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

	registerEvent<EventType::Tick>(this, &Model::tick);
	registerEvent<EventType::Connected>(this, &Model::connected);
	registerEvent<EventType::Disconnected>(this, &Model::disconnected);
	registerEvent<EventType::UpdateAvailable>(this, &Model::newUpdateAvailable);
	registerEvent<EventType::FanData>(this, &Model::newFanData);
	registerEvent<EventType::FileData>(this, &Model::newFileData);
	registerEvent<EventType::HeaterData>(this, &Model::newHeaterData);
	registerEvent<EventType::JobFileName>(this, &Model::newJobFileName);
	registerEvent<EventType::JobLastFileName>(this, &Model::newJobLastFileName);
	registerEvent<EventType::JobPrintTime>(this, &Model::newJobPrintTime);
	registerEvent<EventType::JobDuration>(this, &Model::newJobDuration);
	registerEvent<EventType::JobTimeLeft>(this, &Model::newJobTimeLeft);
	registerEvent<EventType::JobWarmupDuration>(this, &Model::newJobWarmupDuration);
	registerEvent<EventType::JobBuild>(this, &Model::newJobBuild);
	registerEvent<EventType::JobCurrentObject>(this, &Model::newJobCurrentObject);
	registerEvent<EventType::JobObjectData>(this, &Model::newJobObjectData);
	registerEvent<EventType::ThumbnailData>(this, &Model::newThumbnailData);
	registerEvent<EventType::AxesData>(this, &Model::newAxesData);
	registerEvent<EventType::ExtruderData>(this, &Model::newExtruderData);
	registerEvent<EventType::KinematicsName>(this, &Model::newKinematicsName);
	registerEvent<EventType::SpeedFactor>(this, &Model::newSpeedFactor);
	registerEvent<EventType::WorkplaceNumber>(this, &Model::newWorkplaceNumber);
	registerEvent<EventType::PrintingAcceleration>(this, &Model::newPrintingAcceleration);
	registerEvent<EventType::CurrentMoveRequestedSpeed>(this, &Model::newCurrentMoveRequestedSpeed);
	registerEvent<EventType::CurrentMoveTopSpeed>(this, &Model::newCurrentMoveTopSpeed);
	registerEvent<EventType::CurrentMoveExtrusionSpeed>(this, &Model::newCurrentMoveExtrusionSpeed);
	registerEvent<EventType::CompensationFile>(this, &Model::newCompensationFile);
	registerEvent<EventType::Response>(this, &Model::newResponse);
	registerEvent<EventType::LogMessage>(this, &Model::newLogMessage);
	registerEvent<EventType::AnalogSensorData>(this, &Model::newAnalogSensorData);
	registerEvent<EventType::EndstopData>(this, &Model::newEndstopData);
	registerEvent<EventType::SpindleData>(this, &Model::newSpindleData);
	registerEvent<EventType::NetworkName>(this, &Model::newNetworkName);
	registerEvent<EventType::IpAddress>(this, &Model::newIpAddress);
	registerEvent<EventType::Status>(this, &Model::newStatus);
	registerEvent<EventType::CurrentTool>(this, &Model::newCurrentTool);
	registerEvent<EventType::MessageBoxData>(this, &Model::newMessageBoxData);
	registerEvent<EventType::Time>(this, &Model::newTime);
	registerEvent<EventType::ToolData>(this, &Model::newToolData);
	registerEvent<EventType::Directories>(this, &Model::newDirectories);
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

		auto it = m_handlers.find(event.first);
		if (it != m_handlers.end())
		{
			it->second(event.second);
		}
		else
		{
			LOG_WARN("No handler for event type {:d}", (int)event.first);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

MODEL_NOTIFICATION(tick);

void Model::requestNewData()
{
	Comm::sendNext();
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
	NOTIFY_ALL_PRESENTERS(connected);
	post<EventType::Response>(_("connected_message"));
}

void Model::disconnected()
{
	LOG_DBG("Disconnected event");
	NOTIFY_ALL_PRESENTERS(disconnected);
	post<EventType::Response>(_("disconnected_message"));
}

void Model::newUpdateAvailable(const std::string& file)
{
	NOTIFY_ALL_PRESENTERS(newUpdateAvailable, file);
}

/* Directory methods */

MODEL_NOTIFICATION(newDirectories)

/* Fan methods */

MODEL_NOTIFICATION(newFanData)

/* File methods */

MODEL_NOTIFICATION(newFileData)

/* Heater methods */

MODEL_NOTIFICATION(newHeaterData)

/* Job methods */

void Model::newJobFileName(const std::string& filename)
{
	NOTIFY_ALL_PRESENTERS(newJobFileName, filename);
}

void Model::newJobLastFileName(const std::string& filename)
{
	NOTIFY_ALL_PRESENTERS(newJobLastFileName, filename);
}

MODEL_NOTIFICATION(newJobPrintTime)
MODEL_NOTIFICATION(newJobDuration)
MODEL_NOTIFICATION(newJobTimeLeft)
MODEL_NOTIFICATION(newJobWarmupDuration)
MODEL_NOTIFICATION(newJobBuild)
MODEL_NOTIFICATION(newJobCurrentObject)
MODEL_NOTIFICATION(newJobObjectData)

void Model::newThumbnailData(const std::string& filename)
{
	NOTIFY_ALL_PRESENTERS(newThumbnailData, filename);
}

/* Move methods */

MODEL_NOTIFICATION(newAxesData)
MODEL_NOTIFICATION(newExtruderData)

void Model::newKinematicsName(const std::string& kinematicsName)
{
	NOTIFY_ALL_PRESENTERS(newKinematicsName, kinematicsName);
}

MODEL_NOTIFICATION(newSpeedFactor)
MODEL_NOTIFICATION(newWorkplaceNumber)

void Model::newPrintingAcceleration(const uint32_t& accel)
{
	NOTIFY_ALL_PRESENTERS(newPrintingAcceleration, accel);
}

MODEL_NOTIFICATION(newCurrentMoveRequestedSpeed)
MODEL_NOTIFICATION(newCurrentMoveTopSpeed)
MODEL_NOTIFICATION(newCurrentMoveExtrusionSpeed)
MODEL_NOTIFICATION(newCompensationFile)

/* Response methods */

void Model::newResponse(const std::string& resp)
{
	NOTIFY_ALL_PRESENTERS(newResponse, resp);
}

void Model::newLogMessage(const Log::DebugLevel& level, const Log::log_time_t& time, const std::string& message)
{
	NOTIFY_ALL_PRESENTERS(newLogMessage, level, time, message);
}

/* Sensor methods */

MODEL_NOTIFICATION(newAnalogSensorData)
MODEL_NOTIFICATION(newEndstopData)

/* Spindle methods */

MODEL_NOTIFICATION(newSpindleData)

/* State methods */

MODEL_NOTIFICATION(newNetworkName)
MODEL_NOTIFICATION(newIpAddress)

void Model::newStatus(const OM::PrinterStatus status)
{
	NOTIFY_ALL_PRESENTERS(newStatus, status);
}

MODEL_NOTIFICATION(newCurrentTool)

void Model::newMessageBoxData(const OM::Alert& alert)
{
	OM::g_lastAlertSeq = alert.seq;
	NOTIFY_ALL_PRESENTERS(newMessageBoxData, alert);
}

MODEL_NOTIFICATION(newTime)

/* Tool methods */

MODEL_NOTIFICATION(newToolData)

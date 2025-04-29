#include "Model.h"
#include "Comm/JsonDecoder.h"
#include "Comm/Usb.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/PrinterStatus.h"
#include "Presenter.h"
#include "View.h"
#include "lvgl/src/osal/lv_os.h"

#define NOTIFY_ALL_PRESENTERS(func, ...)                                                                               \
  {                                                                                                                    \
	auto it = m_presenters.begin();                                                                                    \
	std::shared_ptr<UI::BasePresenter> presenter;                                                                      \
	while (true)                                                                                                       \
	{                                                                                                                  \
	  {                                                                                                                \
		UI_LOCK();                                                                                                     \
		if (it == m_presenters.end())                                                                                  \
		{                                                                                                              \
		  break;                                                                                                       \
		}                                                                                                              \
		presenter = *it;                                                                                               \
	  }                                                                                                                \
	  presenter->func(__VA_ARGS__);                                                                                    \
	  {                                                                                                                \
		UI_LOCK();                                                                                                     \
		++it;                                                                                                          \
	  }                                                                                                                \
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
	m_timers.tick =
		lv_timer_create([](lv_timer_t* timer) { static_cast<Model*>(lv_timer_get_user_data(timer))->tick(); },
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

	registerEvent<EventType::Message>(this, &Model::message);
}

void Model::message(const std::string& msg)
{
	LOG_INFO("Message: {:s}", msg);
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
	}
}

void Model::tick()
{
	for (auto presenter : m_presenters)
	{
		presenter->tick();
	}
}

void Model::requestNewData()
{
	Comm::sendNext();
}

useconds_t Model::receiveNewUsbData()
{
	static constexpr size_t bufferSize = 32768;
	static Comm::JsonDecoder decoder;
	static BYTE buffer[bufferSize];
	static size_t bufferLen = 0;

	if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::usb)
	{
		return 500 * 1000;
	}

	if (!Comm::getCurrentUsbDevice().isConnected())
	{
		LOG_VERBOSE("USB device disconnected");
		return 500 * 1000;
	}
	int len = Comm::getCurrentUsbDevice().receive(buffer + bufferLen, bufferSize - bufferLen);

	if (len > 0)
	{
		bufferLen += len;
		if (bufferLen >= bufferSize)
		{
			LOG_ERROR("Buffer overflow");
			bufferLen = 0;
			return 5 * 1000;
		}
	}
	else if (len < 0)
	{
		LOG_ERROR("Error receiving data");
		bufferLen = 0;
		return 5 * 1000;
	}
	if (buffer[bufferLen - 1] == '\n')
	{
		// Process the data
		decoder.CheckInput(buffer, bufferLen);
		bufferLen = 0;
	}
	return 5 * 1000;
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

void Model::refresh()
{
	for (auto presenter : m_presenters)
	{
		presenter->refresh();
		presenter->newFanData();
		presenter->newFileData();
		presenter->newHeaterData();
		presenter->newJobFileName(OM::GetJobName().c_str());
		presenter->newJobLastFileName(OM::GetLastJobName().c_str());
		presenter->newJobPrintTime();
		presenter->newJobDuration();
		presenter->newJobTimeLeft();
		presenter->newJobWarmupDuration();
		presenter->newJobBuild();
		presenter->newJobCurrentObject();
		presenter->newJobObjectData();
		presenter->newAxesData();
		presenter->newExtruderData();
		presenter->newKinematicsName();
		presenter->newSpeedFactor();
		presenter->newWorkplaceNumber();
		presenter->newCurrentMoveRequestedSpeed();
		presenter->newCurrentMoveTopSpeed();
		presenter->newCurrentMoveExtrusionSpeed();
		presenter->newCompensationFile();
		presenter->newAnalogSensorData();
		presenter->newEndstopData();
		presenter->newSpindleData();
		presenter->newNetworkName();
		presenter->newIpAddress();
		presenter->newStatus(OM::GetStatus());
		presenter->newCurrentTool();
		presenter->newMessageBoxData(OM::g_currentAlert);
		presenter->newTime();
		presenter->newToolData();
	}
}

void Model::newUpdateAvailable(const std::string& file)
{
	NOTIFY_ALL_PRESENTERS(newUpdateAvailable, file);
}

/* Fan methods */

MODEL_NOTIFICATION(newFanData)

/* File methods */

MODEL_NOTIFICATION(newFileData)

/* Heater methods */

MODEL_NOTIFICATION(newHeaterData)

/* Job methods */

void Model::newJobFileName(const char* filename)
{
	NOTIFY_ALL_PRESENTERS(newJobFileName, filename);
}

void Model::newJobLastFileName(const char* filename)
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

void Model::newThumbnailData(const char* filename)
{
	NOTIFY_ALL_PRESENTERS(newThumbnailData, filename);
}

/* Move methods */

MODEL_NOTIFICATION(newAxesData)
MODEL_NOTIFICATION(newExtruderData)
MODEL_NOTIFICATION(newKinematicsName)
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

void Model::newResponse(const char* resp)
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

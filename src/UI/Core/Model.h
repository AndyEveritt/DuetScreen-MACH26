#pragma once

#include "Debug.h"
#include "LockWrapper.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/PrinterStatus.h"
#include "Subscribers/DirectoriesSubscribers.h"
#include "Subscribers/FanSubscribers.h"
#include "Subscribers/FileSubscribers.h"
#include "Subscribers/HeatSubscribers.h"
#include "Subscribers/JobSubscribers.h"
#include "Subscribers/MoveSubscribers.h"
#include "Subscribers/ResponseSubscribers.h"
#include "Subscribers/SensorSubscribers.h"
#include "Subscribers/SpindleSubscribers.h"
#include "Subscribers/StateSubscribers.h"
#include "Subscribers/Subscribers.h"
#include "Subscribers/ThumbnailSubscribers.h"
#include "Subscribers/ToolSubscribers.h"
#include "lvgl/lvgl.h"
#include <atomic>
#include <condition_variable>
#include <fmt/ostream.h>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <tuple>
#include <variant>

namespace UI
{
	class BasePresenter;
}

enum class EventType
{
	Tick,
	Connected,
	Disconnected,
	UpdateAvailable,
	FanData,
	FileData,
	HeaterData,
	JobFileName,
	JobLastFileName,
	JobPrintTime,
	JobDuration,
	JobTimeLeft,
	JobWarmupDuration,
	JobBuild,
	JobCurrentObject,
	JobObjectData,
	ThumbnailData,
	AxesData,
	ExtruderData,
	KinematicsName,
	SpeedFactor,
	WorkplaceNumber,
	PrintingAcceleration,
	CurrentMoveRequestedSpeed,
	CurrentMoveTopSpeed,
	CurrentMoveExtrusionSpeed,
	CompensationFile,
	Response,
	LogMessage,
	AnalogSensorData,
	EndstopData,
	SpindleData,
	NetworkName,
	IpAddress,
	Status,
	CurrentTool,
	MessageBoxData,
	Time,
	ToolData,
	Directories
};

// This is an attempt to use templates instead of macros but it doesn't currently work
#define DEV_EVENT_TRAIT_TEMPLATE 0

#if DEV_EVENT_TRAIT_TEMPLATE
template <EventType E, typename... Args>
struct EventTraits
{
	using tuple_type = std::tuple<std::decay_t<Args>...>;
	struct EventData
	{
		EventType type = E;
		EventData() {}
		EventData(const tuple_type& t)
			: tup(t)
		{
		}
		EventData(tuple_type&& t)
			: tup(std::move(t))
		{
		}
		tuple_type tup;
	} data;

	EventTraits() {}
	EventTraits(const tuple_type& t)
		: data(t)
	{
	}
	EventTraits(tuple_type&& t)
		: data(std::move(t))
	{
	}
};

using EventData = std::variant<EventTraits<EventType::Tick>,
							   EventTraits<EventType::Refresh>,
							   EventTraits<EventType::UpdateAvailable, std::string>,
							   EventTraits<EventType::FanData>,
							   EventTraits<EventType::FileData>,
							   EventTraits<EventType::HeaterData>,
							   EventTraits<EventType::JobFileName, std::string>,
							   EventTraits<EventType::JobLastFileName, std::string>,
							   EventTraits<EventType::JobPrintTime>,
							   EventTraits<EventType::JobDuration>,
							   EventTraits<EventType::JobTimeLeft>,
							   EventTraits<EventType::JobWarmupDuration>,
							   EventTraits<EventType::JobBuild>,
							   EventTraits<EventType::JobCurrentObject>,
							   EventTraits<EventType::JobObjectData>,
							   EventTraits<EventType::ThumbnailData>,
							   EventTraits<EventType::AxesData>,
							   EventTraits<EventType::ExtruderData>,
							   EventTraits<EventType::KinematicsName>,
							   EventTraits<EventType::SpeedFactor>,
							   EventTraits<EventType::WorkplaceNumber>,
							   EventTraits<EventType::PrintingAcceleration, uint32_t>,
							   EventTraits<EventType::CurrentMoveRequestedSpeed>,
							   EventTraits<EventType::CurrentMoveTopSpeed>,
							   EventTraits<EventType::CurrentMoveExtrusionSpeed>,
							   EventTraits<EventType::CompensationFile>,
							   EventTraits<EventType::Response, std::string>,
							   EventTraits<EventType::LogMessage, Log::DebugLevel, Log::log_time_t, std::string>,
							   EventTraits<EventType::AnalogSensorData>,
							   EventTraits<EventType::EndstopData>,
							   EventTraits<EventType::SpindleData>,
							   EventTraits<EventType::NetworkName>,
							   EventTraits<EventType::IpAddress>,
							   EventTraits<EventType::Status, OM::PrinterStatus>,
							   EventTraits<EventType::CurrentTool>,
							   EventTraits<EventType::MessageBoxData, OM::Alert>,
							   EventTraits<EventType::Time>,
							   EventTraits<EventType::ToolData>,
							   EventTraits<EventType::Directories>>;
#else
template <EventType E>
struct EventTraits
{
};
#  define REGISTER_EVENT_TYPE(type, args...)                                                                           \
	  template <>                                                                                                      \
	  struct EventTraits<type>                                                                                         \
	  {                                                                                                                \
		  using tuple_type = std::tuple<args>;                                                                         \
		  struct EventData                                                                                             \
		  {                                                                                                            \
			  EventType eventType = type;                                                                              \
			  tuple_type tup;                                                                                          \
			  EventData(EventType eventType)                                                                           \
				  : eventType(eventType)                                                                               \
			  {                                                                                                        \
			  }                                                                                                        \
			  EventData(EventType eventType, tuple_type tup)                                                           \
				  : eventType(eventType)                                                                               \
				  , tup(std::move(tup))                                                                                \
			  {                                                                                                        \
			  }                                                                                                        \
		  } data;                                                                                                      \
		  using data_type = EventData;                                                                                 \
		  EventTraits()                                                                                                \
			  : data(type)                                                                                             \
		  {                                                                                                            \
		  }                                                                                                            \
		  EventTraits(tuple_type tup)                                                                                  \
			  : data(type, std::move(tup))                                                                             \
		  {                                                                                                            \
		  }                                                                                                            \
	  };

REGISTER_EVENT_TYPE(EventType::Tick)
REGISTER_EVENT_TYPE(EventType::Connected)
REGISTER_EVENT_TYPE(EventType::Disconnected)
REGISTER_EVENT_TYPE(EventType::UpdateAvailable, std::string)
REGISTER_EVENT_TYPE(EventType::FanData)
REGISTER_EVENT_TYPE(EventType::FileData)
REGISTER_EVENT_TYPE(EventType::HeaterData)
REGISTER_EVENT_TYPE(EventType::JobFileName, std::string)
REGISTER_EVENT_TYPE(EventType::JobLastFileName, std::string)
REGISTER_EVENT_TYPE(EventType::JobPrintTime)
REGISTER_EVENT_TYPE(EventType::JobDuration)
REGISTER_EVENT_TYPE(EventType::JobTimeLeft)
REGISTER_EVENT_TYPE(EventType::JobWarmupDuration)
REGISTER_EVENT_TYPE(EventType::JobBuild)
REGISTER_EVENT_TYPE(EventType::JobCurrentObject)
REGISTER_EVENT_TYPE(EventType::JobObjectData)
REGISTER_EVENT_TYPE(EventType::ThumbnailData, std::string)
REGISTER_EVENT_TYPE(EventType::AxesData)
REGISTER_EVENT_TYPE(EventType::ExtruderData)
REGISTER_EVENT_TYPE(EventType::KinematicsName, std::string)
REGISTER_EVENT_TYPE(EventType::SpeedFactor)
REGISTER_EVENT_TYPE(EventType::WorkplaceNumber)
REGISTER_EVENT_TYPE(EventType::PrintingAcceleration, uint32_t)
REGISTER_EVENT_TYPE(EventType::CurrentMoveRequestedSpeed)
REGISTER_EVENT_TYPE(EventType::CurrentMoveTopSpeed)
REGISTER_EVENT_TYPE(EventType::CurrentMoveExtrusionSpeed)
REGISTER_EVENT_TYPE(EventType::CompensationFile)
REGISTER_EVENT_TYPE(EventType::Response, std::string)
REGISTER_EVENT_TYPE(EventType::LogMessage, Log::DebugLevel, Log::log_time_t, std::string)
REGISTER_EVENT_TYPE(EventType::AnalogSensorData)
REGISTER_EVENT_TYPE(EventType::EndstopData)
REGISTER_EVENT_TYPE(EventType::SpindleData)
REGISTER_EVENT_TYPE(EventType::NetworkName)
REGISTER_EVENT_TYPE(EventType::IpAddress)
REGISTER_EVENT_TYPE(EventType::Status, OM::PrinterStatus)
REGISTER_EVENT_TYPE(EventType::CurrentTool)
REGISTER_EVENT_TYPE(EventType::MessageBoxData, OM::Alert)
REGISTER_EVENT_TYPE(EventType::Time)
REGISTER_EVENT_TYPE(EventType::ToolData)
REGISTER_EVENT_TYPE(EventType::Directories)

// Variant covering all possible event-data tuples
using EventData = std::variant<EventTraits<EventType::Tick>,
							   EventTraits<EventType::Connected>,
							   EventTraits<EventType::Disconnected>,
							   EventTraits<EventType::UpdateAvailable>,
							   EventTraits<EventType::FanData>,
							   EventTraits<EventType::FileData>,
							   EventTraits<EventType::HeaterData>,
							   EventTraits<EventType::JobFileName>,
							   EventTraits<EventType::JobLastFileName>,
							   EventTraits<EventType::JobPrintTime>,
							   EventTraits<EventType::JobDuration>,
							   EventTraits<EventType::JobTimeLeft>,
							   EventTraits<EventType::JobWarmupDuration>,
							   EventTraits<EventType::JobBuild>,
							   EventTraits<EventType::JobCurrentObject>,
							   EventTraits<EventType::JobObjectData>,
							   EventTraits<EventType::ThumbnailData>,
							   EventTraits<EventType::AxesData>,
							   EventTraits<EventType::ExtruderData>,
							   EventTraits<EventType::KinematicsName>,
							   EventTraits<EventType::SpeedFactor>,
							   EventTraits<EventType::WorkplaceNumber>,
							   EventTraits<EventType::PrintingAcceleration>,
							   EventTraits<EventType::CurrentMoveRequestedSpeed>,
							   EventTraits<EventType::CurrentMoveTopSpeed>,
							   EventTraits<EventType::CurrentMoveExtrusionSpeed>,
							   EventTraits<EventType::CompensationFile>,
							   EventTraits<EventType::Response>,
							   EventTraits<EventType::LogMessage>,
							   EventTraits<EventType::AnalogSensorData>,
							   EventTraits<EventType::EndstopData>,
							   EventTraits<EventType::SpindleData>,
							   EventTraits<EventType::NetworkName>,
							   EventTraits<EventType::IpAddress>,
							   EventTraits<EventType::Status>,
							   EventTraits<EventType::CurrentTool>,
							   EventTraits<EventType::MessageBoxData>,
							   EventTraits<EventType::Time>,
							   EventTraits<EventType::ToolData>,
							   EventTraits<EventType::Directories>>;
#endif

using EventCallback = std::function<void(const EventData&)>;

class Model
{
  public:
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	static Model& get()
	{
		static Model instance;
		return instance;
	}

	/**
	 * @brief Add a `Presenter` to listen to events
	 * @param presenter
	 */
	void bind(std::shared_ptr<UI::BasePresenter> presenter);

	/**
	 * @brief Remove a `Presenter`
	 * @param presenter
	 */
	void unbind(std::shared_ptr<UI::BasePresenter> presenter);

	void startEventLoop();
	void stopEventLoop();

	template <EventType E, typename Func>
	void addEventListener(Func&& func)
	{
		m_handlers[E].emplace_back(
			[f = std::forward<Func>(func)](const EventData& data)
			{
				auto& tup = std::get<EventTraits<E>>(data).data.tup;
				std::apply(f, tup);
			});
	}

	template <EventType E, typename Class, typename... Args>
	void addEventListener(Class* instance, void (Class::*memberFunc)(Args...))
	{
		m_handlers[E].emplace_back(
			[instance, memberFunc](const EventData& data)
			{
#if DEV_EVENT_TRAIT_TEMPLATE
				auto& tup = std::get<EventTraits<E, Args...>>(data).data.tup;
#else
				auto& tup = std::get<EventTraits<E>>(data).data.tup;
#endif
				std::apply([instance, memberFunc](const auto&... args) { (instance->*memberFunc)(args...); }, tup);
			});
	}

	template <EventType E, typename... Args>
	void post(Args&&... args)
	{
#if DEV_EVENT_TRAIT_TEMPLATE
		using Traits = EventTraits<E, Args...>;
#else
		using Traits = EventTraits<E>;
#endif
		using decayed_tuple = typename Traits::tuple_type;

		std::lock_guard<std::mutex> lock(m_mutex);
		m_eventQueue.emplace(E, Traits(decayed_tuple(std::forward<Args>(args)...)));
		m_eventCondition.notify_one();
	}

	void runEventLoop();

	/* tasks */
	useconds_t requestNewData();
	useconds_t receiveNewUsbData();

	/* Subscribers */

	void runSubscribers(const char* key, Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	const std::vector<Subscriber>& getSubscribers(const char* key) { return SubscriberMap::getSubscribers(key); }
	const size_t getSubscriberCount(const char* key) { return SubscriberMap::getSubscriberCount(key); }

	void runArrayEndSubscribers(const char* key, Comm::JsonDecoder* decoder, const size_t indices[]);
	const std::vector<ArrayEndSubscriber>& getArrayEndSubscribers(const char* key)
	{
		return SubscriberMap::getArrayEndSubscribers(key);
	}
	const size_t getArrayEndSubscriberCount(const char* key) { return SubscriberMap::getArrayEndSubscriberCount(key); }

  private:
	Model();

	void tick();

	/* presenter callbacks */

	void connected();
	void disconnected();

	void newUpdateAvailable(const std::string& file);

	/* Directory methods */
	void newDirectories();

	/* Fan methods */

	void newFanData();

	/* File methods */

	void newFileData();

	/* Heater methods */

	void newHeaterData();

	/* Job methods */

	void newJobFileName(const std::string& filename);
	void newJobLastFileName(const std::string& filename);
	void newJobPrintTime();
	void newJobDuration();
	void newJobTimeLeft();
	void newJobWarmupDuration();
	void newJobBuild();
	void newJobCurrentObject();
	void newJobObjectData();
	void newThumbnailData(const std::string& filename);

	/* Move methods */

	void newAxesData();
	void newExtruderData();
	void newKinematicsName(const std::string& kinematicsName);
	void newSpeedFactor();
	void newWorkplaceNumber();
	void newPrintingAcceleration(const uint32_t& accel);
	void newCurrentMoveRequestedSpeed();
	void newCurrentMoveTopSpeed();
	void newCurrentMoveExtrusionSpeed();
	void newCompensationFile();

	/* Response methods */

	void newResponse(const std::string& resp);
	void newLogMessage(const Log::DebugLevel& level, const Log::log_time_t& time, const std::string& message);

	/* Sensor methods */

	void newAnalogSensorData();
	void newEndstopData();

	/* Spindle methods */

	void newSpindleData();

	/* State methods */

	void newNetworkName();
	void newIpAddress();
	void newStatus(const OM::PrinterStatus status);
	void newCurrentTool();
	void newMessageBoxData(const OM::Alert& alert);
	void newTime();

	/* Tool methods */

	void newToolData();

	DirectoriesSubscribers m_directoriesSubscribers;
	FanSubscribers m_fanSubscribers;
	FileSubscribers m_fileSubscribers;
	HeatSubscribers m_heatSubscribers;
	JobSubscribers m_jobSubscribers;
	MoveSubscribers m_moveSubscribers;
	ResponseSubscribers m_responseSubscribers;
	SensorSubscribers m_sensorSubscribers;
	SpindleSubscribers m_spindleSubscribers;
	StateSubscribers m_stateSubscribers;
	ThumbnailSubscribers m_thumbnailSubscribers;
	ToolSubscribers m_toolSubscribers;
	std::list<std::shared_ptr<UI::BasePresenter>> m_presenters;

	std::queue<std::pair<EventType, EventData>> m_eventQueue;
	std::map<EventType, std::vector<EventCallback>> m_handlers;
	std::condition_variable m_eventCondition;
	std::thread m_eventThread;
	std::atomic<bool> m_running{false};
	std::mutex m_mutex;

	struct
	{
		lv_timer_t* tick;
		lv_timer_t* request;
		lv_timer_t* receive;
	} m_timers;
};

#define MODEL_LOCK()                                                                                                   \
	LOG_VERBOSE("MODEL_LOCK requested in thread {}", Log::GetThreadId());                                              \
	auto modelLock = ScopedLock(mutexModel);

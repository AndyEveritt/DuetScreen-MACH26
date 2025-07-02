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

enum class EventType;

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

/*
To add an Event, add a new line with the format `XX(EventName, EventArgs...)`

An event can be triggered with `Model::get().post<EventType::EventName>(args...)`.
The event will be passed to the listeners that have registered for the event type.

Any variables that are pass to the event must be copyable.
Events are queued and processed in the order they are received.

Event listeners must be none blocking and should not take a long time to process.
*/
#define EVENTS(XX)                                                                                                     \
	XX(Tick)                                                                                                           \
	XX(Connected)                                                                                                      \
	XX(Disconnected)                                                                                                   \
	XX(UpdateAvailable, std::string)                                                                                   \
	XX(FanData)                                                                                                        \
	XX(FileData)                                                                                                       \
	XX(HeaterData)                                                                                                     \
	XX(JobFileName, std::string)                                                                                       \
	XX(JobLastFileName, std::string)                                                                                   \
	XX(JobPrintTime)                                                                                                   \
	XX(JobDuration)                                                                                                    \
	XX(JobTimeLeft)                                                                                                    \
	XX(JobWarmupDuration)                                                                                              \
	XX(JobHeight)                                                                                                      \
	XX(JobBuild)                                                                                                       \
	XX(JobCurrentObject)                                                                                               \
	XX(JobObjectData)                                                                                                  \
	XX(ThumbnailData, std::string)                                                                                     \
	XX(AxesData)                                                                                                       \
	XX(ExtruderData)                                                                                                   \
	XX(KinematicsName, std::string)                                                                                    \
	XX(SpeedFactor)                                                                                                    \
	XX(WorkplaceNumber)                                                                                                \
	XX(PrintingAcceleration, uint32_t)                                                                                 \
	XX(CurrentMoveRequestedSpeed)                                                                                      \
	XX(CurrentMoveTopSpeed)                                                                                            \
	XX(CurrentMoveExtrusionSpeed)                                                                                      \
	XX(CompensationFile)                                                                                               \
	XX(Response, std::string)                                                                                          \
	XX(LogMessage, Log::DebugLevel, Log::log_time_t, std::string)                                                      \
	XX(AnalogSensorData)                                                                                               \
	XX(EndstopData)                                                                                                    \
	XX(SpindleData)                                                                                                    \
	XX(NetworkName)                                                                                                    \
	XX(IpAddress, std::string)                                                                                         \
	XX(Status, OM::PrinterStatus)                                                                                      \
	XX(CurrentTool)                                                                                                    \
	XX(MessageBoxData, OM::Alert)                                                                                      \
	XX(Time)                                                                                                           \
	XX(ToolData)                                                                                                       \
	XX(Directories)

enum class EventType
{
#define XX(name, ...) name,
	EVENTS(XX)
#undef XX
	Null
};

#define XX(name, ...) EventTraits<EventType::name __VA_OPT__(, ) __VA_ARGS__>,
using EventData = std::variant<EVENTS(XX) EventTraits<EventType::Null>>;
#undef XX

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
				auto& tup = std::get<EventTraits<E, std::decay_t<Args>...>>(data).data.tup;
				std::apply([instance, memberFunc](const auto&... args) { (instance->*memberFunc)(args...); }, tup);
			});
	}

	template <EventType E, typename... Args>
	void post(Args&&... args)
	{
		using Traits = EventTraits<E, std::decay_t<Args>...>;
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

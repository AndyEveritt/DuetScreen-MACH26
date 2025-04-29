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
	Sum,
	Message,
	Heartbeat,
	Empty,
};

// Default traits: no-argument event
template <EventType E>
struct EventTraits
{
};
#define REGISTER_EVENT_TYPE(type, args...)                                                                             \
	template <>                                                                                                        \
	struct EventTraits<type>                                                                                           \
	{                                                                                                                  \
		struct EventData                                                                                               \
		{                                                                                                              \
			EventType eventType = type;                                                                                \
			std::tuple<args> tup;                                                                                      \
			EventData(EventType eventType)                                                                             \
				: eventType(eventType)                                                                                 \
			{                                                                                                          \
			}                                                                                                          \
			EventData(EventType eventType, std::tuple<args> tup)                                                       \
				: eventType(eventType)                                                                                 \
				, tup(std::move(tup))                                                                                  \
			{                                                                                                          \
			}                                                                                                          \
		} data;                                                                                                        \
		using data_type = EventData;                                                                                   \
		EventTraits()                                                                                                  \
			: data(type)                                                                                               \
		{                                                                                                              \
		}                                                                                                              \
		EventTraits(std::tuple<args> tup)                                                                              \
			: data(type, std::move(tup))                                                                               \
		{                                                                                                              \
		}                                                                                                              \
	};

// Specialize for events with arguments
REGISTER_EVENT_TYPE(EventType::Sum, int, int)
REGISTER_EVENT_TYPE(EventType::Heartbeat)
REGISTER_EVENT_TYPE(EventType::Empty)
REGISTER_EVENT_TYPE(EventType::Message, std::string)

// Variant covering all possible event-data tuples
using EventData = std::variant<EventTraits<EventType::Sum>,
							   EventTraits<EventType::Message>,
							   EventTraits<EventType::Heartbeat>,
							   EventTraits<EventType::Empty>>;

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
	void registerEvent(Func&& func)
	{
		m_handlers[E] = [f = std::forward<Func>(func)](const EventData& data)
		{
			auto& tup = std::get<EventTraits<E>>(data).data.tup;
			std::apply(f, tup);
		};
	}

	template <EventType E, typename Class, typename... Args>
	void registerEvent(Class* instance, void (Class::*memberFunc)(Args...))
	{
		m_handlers[E] = [instance, memberFunc](const EventData& data)
		{
			auto& tup = std::get<EventTraits<E>>(data).data.tup;
			std::apply([instance, memberFunc](const auto&... args) { (instance->*memberFunc)(args...); }, tup);
		};
	}

	template <EventType E, typename... Args>
	void post(Args&&... args)
	{
		using Data = EventTraits<E>;
		// static_assert(std::is_constructible<Data::data_type, Args...>::value,
		// 			  "Event data type does not match the provided arguments");
		// static_assert(std::is_convertible<Data, EventData>::value, "Event data type is not convertible to
		// EventData");

		std::lock_guard<std::mutex> lock(m_mutex);
		m_eventQueue.emplace(E, Data(std::make_tuple(std::forward<Args>(args)...)));
		m_eventCondition.notify_one();
	}

	void runEventLoop();

	void message(const std::string& message);
	void heartbeat();

	/* tasks */

	void tick();
	void requestNewData();
	useconds_t receiveNewUsbData();

	/* presenter callbacks */

	void refresh();

	void newUpdateAvailable(const std::string& file);

	/* Fan methods */

	void newFanData();

	/* File methods */

	void newFileData();

	/* Heater methods */

	void newHeaterData();

	/* Job methods */

	void newJobFileName(const char* filename);
	void newJobLastFileName(const char* filename);
	void newJobPrintTime();
	void newJobDuration();
	void newJobTimeLeft();
	void newJobWarmupDuration();
	void newJobBuild();
	void newJobCurrentObject();
	void newJobObjectData();
	void newThumbnailData(const char* filename);

	/* Move methods */

	void newAxesData();
	void newExtruderData();
	void newKinematicsName();
	void newSpeedFactor();
	void newWorkplaceNumber();
	void newPrintingAcceleration(const uint32_t& accel);
	void newCurrentMoveRequestedSpeed();
	void newCurrentMoveTopSpeed();
	void newCurrentMoveExtrusionSpeed();
	void newCompensationFile();

	/* Response methods */

	void newResponse(const char* resp);
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

	void lock();
	void unlock();

  private:
	Model();
	bool initMutex();

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
	std::map<EventType, std::function<void(const EventData&)>> m_handlers;
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

#pragma once

#include "Debug.h"
#include "LockWrapper.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/PrinterStatus.h"
#include "Subscribers/BoardSubscribers.h"
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
#include "nameof.hpp"
#include "tracy/Tracy.hpp"
#include <atomic>
#include <condition_variable>
#include <fmt/ostream.h>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace UI
{
	class BasePresenter;
}

enum class EventType;

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
	XX(BedHeaterData)                                                                                                  \
	XX(ChamberHeaterData)                                                                                              \
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
	XX(NoMoveBeforeHoming)                                                                                             \
	XX(PrintingAcceleration, uint32_t)                                                                                 \
	XX(CurrentMoveRequestedSpeed)                                                                                      \
	XX(CurrentMoveTopSpeed)                                                                                            \
	XX(CurrentMoveExtrusionSpeed)                                                                                      \
	XX(CompensationFile)                                                                                               \
	XX(Response, ResponseType, std::string)                                                                            \
	XX(LogMessage, Log::DebugLevel, Log::log_time_t, std::string)                                                      \
	XX(AnalogSensorData)                                                                                               \
	XX(EndstopData)                                                                                                    \
	XX(SpindleData)                                                                                                    \
	XX(NetworkName)                                                                                                    \
	XX(PrinterUniqueId)                                                                                                \
	XX(IpAddress, std::string)                                                                                         \
	XX(Status, OM::PrinterStatus)                                                                                      \
	XX(CurrentTool)                                                                                                    \
	XX(Alert, OM::Alert)                                                                                               \
	XX(Time)                                                                                                           \
	XX(ToolData)                                                                                                       \
	XX(ToolHeaterData, size_t)                                                                                         \
	XX(Directories)                                                                                                    \
	XX(NavigationHomeEnable, bool)                                                                                     \
	XX(NavigationBackEnable, bool)                                                                                     \
	XX(Filaments, std::vector<std::string>) // New filaments available after rr_filelist/M20 request

enum class EventType
{
#define XX(name, ...) name,
	EVENTS(XX)
#undef XX
	Null
};

// Primary template (unused, will be specialized by macro below)
template <EventType E>
struct EventTraits;

// Specializations generated from EVENTS macro: maps EventType -> argument tuple
#define XX(name, ...)                                                                                                  \
	template <>                                                                                                        \
	struct EventTraits<EventType::name>                                                                                \
	{                                                                                                                  \
		using tuple_type = std::tuple<__VA_ARGS__>;                                                                    \
	};

EVENTS(XX)
#undef XX

// Provide traits for Null event
template <>
struct EventTraits<EventType::Null>
{
	using tuple_type = std::tuple<>;
};

// Wrapper that stores the payload tuple for a given EventType
template <EventType E>
struct EventWrapper
{
	using tuple_type = typename EventTraits<E>::tuple_type;
	EventWrapper() = default;
	explicit EventWrapper(const tuple_type& t)
		: tup(t)
	{
	}
	explicit EventWrapper(tuple_type&& t)
		: tup(std::move(t))
	{
	}
	tuple_type tup;
	constexpr static std::string_view eventName = nameof::nameof_enum<E>();
};

// Variant holding all event payload wrappers
#define XX(name, ...) EventWrapper<EventType::name>,
using EventData = std::variant<EVENTS(XX) EventWrapper<EventType::Null>>;
#undef XX

using EventCallback = std::function<void(const EventData&)>;

class Model
{
  public:
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	static Model& get()
	{
		ZoneScoped;
		static Model instance;
		return instance;
	}

	/**
	 * @brief Add a `Presenter` to listen to events
	 * @param presenter
	 */
	void bind(std::weak_ptr<UI::BasePresenter> presenter);

	/**
	 * @brief Remove a `Presenter`
	 * @param presenter
	 */
	void unbind(std::weak_ptr<UI::BasePresenter> presenter);

	void startEventLoop();
	void stopEventLoop();

	template <EventType E, typename Func>
	void addEventListener(Func&& func)
	{
		ZoneScoped;
		// Verify at compile time that the callable can be invoked with the event's argument list
		using Tuple = typename EventTraits<E>::tuple_type;
		// Expanded check (kept separate for a clean message)
		[]<typename F, typename T, std::size_t... I>(F&&, T*, std::index_sequence<I...>)
		{
			using tuple_t = T;
			using std::get; // not actually used, just to silence unused warnings in some compilers
			static_assert(std::is_invocable_v<F&, std::tuple_element_t<I, tuple_t>&...>,
						  "addEventListener: handler not invocable with event parameter types");
		}(std::forward<Func>(func),
		  static_cast<Tuple*>(nullptr),
		  std::make_index_sequence<std::tuple_size<Tuple>::value>{});

		m_handlers[E].emplace_back(
			[f = std::forward<Func>(func)](const EventData& data)
			{
				auto& tup = std::get<EventWrapper<E>>(data).tup;
				std::apply(f, tup);
			});
	}

	template <EventType E, typename Class, typename... Args>
	void addEventListener(Class* instance, void (Class::*memberFunc)(Args...))
	{
		ZoneScoped;
		// Compile-time verification that the member function signature matches the event's argument list
		using ExpectedTuple = typename EventTraits<E>::tuple_type;
		using ProvidedTuple = std::tuple<std::decay_t<Args>...>;
		static_assert(std::is_same_v<ExpectedTuple, ProvidedTuple>,
					  "addEventListener member function args mismatch for event");

		m_handlers[E].emplace_back(
			[instance, memberFunc](const EventData& data)
			{
				auto& tup = std::get<EventWrapper<E>>(data).tup;
				std::apply([instance, memberFunc](auto const&... a) { (instance->*memberFunc)(a...); }, tup);
			});
	}

	template <EventType E, typename... Args>
	void post(Args&&... args)
	{
		[[maybe_unused]] constexpr auto eventName = nameof::nameof_enum<E>();
		ZoneScopedNC(eventName.data(), tracy::Color::Red);
		using Wrapper = EventWrapper<E>;
		using Tuple = typename Wrapper::tuple_type;
		using ExpectedTuple = Tuple;
		using ProvidedTuple = std::tuple<std::decay_t<Args>...>;
		static_assert(std::is_same_v<ExpectedTuple, ProvidedTuple>,
					  "post() argument types mismatch for event, expected");

		std::lock_guard<LockableBase(std::mutex)> lock(m_mutex);
		m_eventQueue.emplace(E, EventData(std::in_place_type<Wrapper>, Tuple(std::forward<Args>(args)...)));
		m_eventCondition.notify_one();
	}

	void runEventLoop();

	/* tasks */
	std::chrono::milliseconds requestNewData();

	/* Subscribers */

	void runSubscribers(const char* key, Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	const std::vector<Subscriber>& getSubscribers(const char* key) { return SubscriberMap::getSubscribers(key); }
	size_t getSubscriberCount(const char* key) { return SubscriberMap::getSubscriberCount(key); }

	void runArrayEndSubscribers(const char* key, Comm::JsonDecoder* decoder, const size_t indices[]);
	const std::vector<ArrayEndSubscriber>& getArrayEndSubscribers(const char* key)
	{
		return SubscriberMap::getArrayEndSubscribers(key);
	}
	size_t getArrayEndSubscriberCount(const char* key) { return SubscriberMap::getArrayEndSubscriberCount(key); }

  private:
	Model();

	void tick();

	/* presenter callbacks */

	void connected();
	void disconnected();

	BoardSubscribers m_boardSubscribers;
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
	std::list<std::weak_ptr<UI::BasePresenter>> m_presenters;

	// Optimized event dispatch: map event -> presenters subscribed to that event
	std::unordered_map<EventType, std::vector<std::weak_ptr<UI::BasePresenter>>> m_eventPresenterIndex;
	// Reverse index to support fast unbind cleanup
	std::unordered_map<UI::BasePresenter*, std::vector<EventType>> m_presenterEventIndex;

	std::queue<std::pair<EventType, EventData>> m_eventQueue;
	std::map<EventType, std::vector<EventCallback>> m_handlers;
	std::condition_variable_any m_eventCondition;
	std::thread m_eventThread;
	std::atomic<bool> m_running{false};
	TracyLockable(std::mutex, m_mutex);

	struct
	{
		lv_timer_t* tick;
		lv_timer_t* request;
		lv_timer_t* receive;
	} m_timers;
};

#define MODEL_LOCK()                                                                                                   \
	LOG_VERBOSE("MODEL_LOCK requested in thread {}", Log::GetThreadId());                                              \
	std::lock_guard<LockableBase(DeadlockDetectingMutex<std::recursive_mutex>)> modelLock(mutexModel);

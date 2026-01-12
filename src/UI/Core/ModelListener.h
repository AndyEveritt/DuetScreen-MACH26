#pragma once

#include "Model.h"

namespace UI
{
	class ModelListener
	{
	  public:
		ModelListener()
			: m_model(Model::get())
		{
		}

		virtual ~ModelListener() {}

		Model& getModel() const { return m_model; }

		EventCallback getEventHandler(EventType eventType)
		{
			auto it = m_handlers.find(eventType);
			if (it != m_handlers.end())
			{
				return it->second;
			}
			return nullptr;
		}

		const auto& getEventHandlers() const { return m_handlers; }

	  protected:
		template <EventType E, typename Class, typename... Args>
		void registerEventListener(Class* instance, void (Class::*memberFunc)(Args...))
		{
			using ExpectedTuple = typename EventTraits<E>::tuple_type;
			using ProvidedTuple = std::tuple<std::decay_t<Args>...>;
			static_assert(std::is_same_v<ExpectedTuple, ProvidedTuple>, "registerEventListener member args mismatch");
			m_handlers[E] = [instance, memberFunc](const EventData& data)
			{
				auto& tup = std::get<EventWrapper<E>>(data).tup;
				std::apply([instance, memberFunc](const auto&... args) { (instance->*memberFunc)(args...); }, tup);
			};
		}

		// Register a free function / lambda / functor for event E. Argument types are deduced
		// from the event definition; no need to spell them out at the call site.
		// Example: registerEventListener<EventType::UpdateAvailable>([](const std::string& s){ ... });
		template <EventType E, typename Func>
		void registerEventListener(Func&& func)
		{
			using Tuple = typename EventTraits<E>::tuple_type;

			/* Validate func is callable with the correct arguments */
			[]<typename F, typename T, std::size_t... I>(F&&, T*, std::index_sequence<I...>)
			{
				static_assert(std::is_invocable_v<F&, std::tuple_element_t<I, T>&...>,
							  "registerEventListener: handler not invocable with event parameter types");
			}(std::forward<Func>(func),
			  static_cast<Tuple*>(nullptr),
			  std::make_index_sequence<std::tuple_size<Tuple>::value>{});

			m_handlers[E] = [f = std::forward<Func>(func)](const EventData& data)
			{
				auto& tup = std::get<EventWrapper<E>>(data).tup;
				std::apply(f, tup);
			};
		}

		Model& m_model;

		std::unordered_map<EventType, EventCallback> m_handlers;
	};
} // namespace UI

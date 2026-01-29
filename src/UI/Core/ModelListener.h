#pragma once

#include "Model.h"
#include <memory>
#include <vector>
#include <tuple>
#include <unordered_map>

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

		EventHandler getEventHandler(EventType eventType)
		{
			auto it = m_handlers.find(eventType);
			if (it != m_handlers.end())
			{
				return it->second;
			}
			return EventHandler{};
		}

		const auto& getEventHandlers() const { return m_handlers; }

	  protected:
		template <EventType E, typename Class, typename... Args>
			requires UI::ExactArgsMatch<typename EventTraits<E>::tuple_type, Args...>
		void registerEventListener(Class* instance, void (Class::*memberFunc)(Args...))
		{
			using ExpectedTuple = typename EventTraits<E>::tuple_type;

			struct Ctx
			{
				Class* instance;
				void (Class::*mf)(Args...);
			};
			// Store context locally to keep lifetime tied to presenter.
			auto ctxPtr = std::make_unique<Ctx>(Ctx{instance, memberFunc});
			m_ctxStorage.emplace_back(ctxPtr.release(), Deleter{+[](void* p) { delete static_cast<Ctx*>(p); }});
			void* user = m_ctxStorage.back().get();

			auto thunk = [](void* user, const void* payload)
			{
				const auto& tup = *static_cast<const ExpectedTuple*>(payload);
				auto* c = static_cast<Ctx*>(user);
				std::apply([c](const auto&... a) { (c->instance->*c->mf)(a...); }, tup);
			};
			m_handlers[E] = EventHandler{thunk, user};
		}

		// Register a free function / lambda / functor for event E.
		template <EventType E, typename Func>
			requires UI::InvocableFromTuple<std::decay_t<Func>, typename EventTraits<E>::tuple_type>
		void registerEventListener(Func&& func)
		{
			using Tuple = typename EventTraits<E>::tuple_type;

			using F = std::decay_t<Func>;
			auto fPtr = std::make_unique<F>(std::forward<Func>(func));
			m_ctxStorage.emplace_back(fPtr.release(), Deleter{+[](void* p) { delete static_cast<F*>(p); }});
			void* user = m_ctxStorage.back().get();
			auto thunk = [](void* user, const void* payload)
			{
				const auto& tup = *static_cast<const Tuple*>(payload);
				std::apply(*static_cast<F*>(user), tup);
			};
			m_handlers[E] = EventHandler{thunk, user};
		}

		Model& m_model;

		std::unordered_map<EventType, EventHandler> m_handlers;
		// Keep storage for handler contexts alive for the lifetime of the presenter
		struct Deleter { void (*fn)(void*); void operator()(void* p) const { if (fn) fn(p); } };
		std::vector<std::unique_ptr<void, Deleter>> m_ctxStorage;
	};
} // namespace UI

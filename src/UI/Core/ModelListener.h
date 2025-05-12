#pragma once

#include "Model.h"
#include "ObjectModel/Alert.h"

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

	  protected:
		template <EventType E, typename Class, typename... Args>
		void registerEventListener(Class* instance, void (Class::*memberFunc)(Args...))
		{
			m_handlers[E] = [instance, memberFunc](const EventData& data)
			{
#if DEV_EVENT_TRAIT_TEMPLATE
				auto& tup = std::get<EventTraits<E, Args...>>(data).data.tup;
#else
				auto& tup = std::get<EventTraits<E>>(data).data.tup;
#endif
				std::apply([instance, memberFunc](const auto&... args) { (instance->*memberFunc)(args...); }, tup);
			};
		}

		Model& m_model;

		std::map<EventType, EventCallback> m_handlers;
	};
} // namespace UI

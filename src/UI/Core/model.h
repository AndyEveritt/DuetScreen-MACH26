#pragma once

#include "subscribers.h"
#include <map>

namespace UI
{
	class Presenter;

	class Model
	{
	  public:
		Model()
			: m_presenter(0)
		{
		}

		void bind(Presenter* presenter) { m_presenter = presenter; }

		void tick();
		void updateView();

		const std::vector<subscriberCb_t>& getSubscribers(const char* key);
		size_t getSubscriberCount(const char* key);

	  protected:
		// Add a callback to be run when json matching key is received, if the asociated view is active
		bool addSubscriber(const char* key, subscriberCb_t cb);
		bool removeSubscriber(const char* key, size_t index);

		std::map<const char*, std::vector<subscriberCb_t>> m_subscribers;
		Presenter* m_presenter;
	};
} // namespace UI

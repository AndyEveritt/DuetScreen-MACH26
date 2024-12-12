#pragma once

#include "modellistener.h"
#include "subscribers.h"
#include <map>
#include <vector>

namespace UI
{

	class Presenter : ModelListener
	{
	  public:
		virtual void activate() {}
		virtual void deactivate() {}

		const std::vector<subscriberCb_t>& getSubscribers(const char* key);
		size_t getSubscriberCount(const char* key);

	  protected:
		// Add a callback to be run when json matching key is received, if the asociated view is active
		bool addSubscriber(const char* key, subscriberCb_t cb);
		bool removeSubscriber(const char* key, size_t index);

		std::map<const char*, std::vector<subscriberCb_t>> m_subscribers;
	};
} // namespace UI

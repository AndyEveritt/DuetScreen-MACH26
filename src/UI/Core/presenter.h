#pragma once

#include "modellistener.h"
#include "subscribers.h"
#include <map>
#include <vector>

namespace UI
{
	class BasePresenter : public ModelListener
	{
	  public:
		void activate()
		{
			m_model->bind(this);
			onActivate();
		}
		void deactivate()
		{
			m_model->unbind(this);
			onDeactivate();
		}

		const std::vector<subscriberCb_t>& getSubscribers(const char* key);
		size_t getSubscriberCount(const char* key);

	  protected:
		virtual void onActivate() {}
		virtual void onDeactivate() {}

		// Add a callback to be run when json matching key is received, if the asociated view is active
		bool addSubscriber(const char* key, subscriberCb_t cb);
		bool removeSubscriber(const char* key, size_t index);

		std::map<const char*, std::vector<subscriberCb_t>> m_subscribers;
	};

	template <class V>
	class Presenter : public BasePresenter
	{
	  public:
		Presenter()
			: m_view(nullptr)
		{
		}
		Presenter(V* view)
			: m_view(view)
		{
		}

	  protected:
		V* m_view;
	};
} // namespace UI

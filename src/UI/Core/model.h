#pragma once

#include "subscribers.h"
#include <list>
#include <map>

namespace UI
{
	class BasePresenter;

	class Model
	{
	  public:
		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static Model& getInstance()
		{
			static Model instance;
			return instance;
		}

		/**
		 * @brief Add a `Presenter` to listen to events
		 * @param presenter
		 */
		void bind(BasePresenter* presenter) { m_presenters.push_back(presenter); }
		/**
		 * @brief Remove a `Presenter`
		 * @param presenter
		 */
		void unbind(BasePresenter* presenter) { m_presenters.remove(presenter); }

		void tick();

		const std::vector<subscriberCb_t>& getSubscribers(const char* key);
		size_t getSubscriberCount(const char* key);

	  protected:
		// Add a callback to be run when json matching key is received, if the asociated view is active
		bool addSubscriber(const char* key, subscriberCb_t cb);
		bool removeSubscriber(const char* key, size_t index);

		std::map<const char*, std::vector<subscriberCb_t>> m_subscribers;
		std::list<BasePresenter*> m_presenters;

	  private:
		Model() {}
	};
} // namespace UI

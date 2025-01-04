#pragma once

#include "UI/Subscribers/FanSubscribers.h"
#include "subscribers.h"
#include <list>
#include <map>

namespace UI
{
	class BasePresenter;
}

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
	void bind(UI::BasePresenter* presenter) { m_presenters.push_back(presenter); }
	/**
	 * @brief Remove a `Presenter`
	 * @param presenter
	 */
	void unbind(UI::BasePresenter* presenter) { m_presenters.remove(presenter); }

	void tick();

	const std::vector<Subscriber>& getSubscribers(const char* key) { return SubscriberMap::getSubscribers(key); }
	size_t getSubscriberCount(const char* key) { return SubscriberMap::getSubscriberCount(key); }

  private:
	FanSubscribers m_fanSubscribers;
	std::list<UI::BasePresenter*> m_presenters;

  private:
	Model() {}
};

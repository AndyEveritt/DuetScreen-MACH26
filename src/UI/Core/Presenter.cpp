/*
 * Presenter.cpp
 *
 *  Created on: 2025-07-14
 *      Author: Andy Everitt
 */

#include "Presenter.h"
#include "Debug.h"

namespace UI
{
	void BasePresenter::init()
	{
		LOG_DBG("Initializing presenter '{}'", getName());
		onInit();
		registerEventListener<EventType::Disconnected>(this, &BasePresenter::disconnected);
	}

	void BasePresenter::activate()
	{
		LOG_DBG("Activating presenter '{}'", getName());
		onActivate();
		m_active = true;
	}

	void BasePresenter::deactivate()
	{
		LOG_DBG("Deactivating presenter '{}'", getName());
		onDeactivate();
		m_active = false;
	}

	const std::string& BasePresenter::getName() const
	{
		static std::string name = "BasePresenter";
		return name;
	}

	void BasePresenter::disconnected()
	{
		LOG_DBG("{} disconnected", getName());
		onDisconnect();
	}
} // namespace UI

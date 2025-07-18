/*
 * Presenter.cpp
 *
 *  Created on: 2025-07-14
 *      Author: Andy Everitt
 */

#include "Presenter.h"
#include "Debug.h"
#include "UI/Components/LVGL/LvObj.h"

namespace UI
{
	void BasePresenter::init()
	{
		LOG_DBG("Initializing presenter '{}'", getName());
		onInit();
		registerEventListener<EventType::Connected>(this, &BasePresenter::connected);
		registerEventListener<EventType::Disconnected>(this, &BasePresenter::disconnected);
	}

	void BasePresenter::activate()
	{
		UI_LOCK();
		LOG_DBG("Activating presenter '{}'", getName());
		if (!m_active)
		{
			onActivate();
			m_active = true;
		}
	}

	void BasePresenter::deactivate()
	{
		UI_LOCK();
		LOG_DBG("Deactivating presenter '{}'", getName());
		if (m_active)
		{
			onDeactivate();
			m_active = false;
		}
	}

	std::string_view BasePresenter::getName() const
	{
		static std::string_view name = "BasePresenter";
		return name;
	}

	void BasePresenter::connected()
	{
		LOG_DBG("{} connected", getName());
		onConnect();
		onActivate();
	}

	void BasePresenter::disconnected()
	{
		LOG_DBG("{} disconnected", getName());
		onDisconnect();
	}
} // namespace UI

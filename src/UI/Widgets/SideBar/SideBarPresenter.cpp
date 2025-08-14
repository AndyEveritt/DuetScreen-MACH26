/*
 * SideBarPresenter.cpp
 *
 *  Created on: 2025-02-21
 *      Author: Andy Everitt
 */

#include "SideBarPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "SideBar.h"
#include <string>
#include <thread>

using namespace std::string_literals;

namespace UI
{
	void SideBarPresenter::eStop()
	{
		LOG_WARN("EStop Pressed!");
		Comm::DUET.SendGcode("M112 M999\n");
		LOG_WARN("Emergency Stop sent to Duet");
		Comm::DUET.Disconnect();
		Model::get().post<EventType::Response>("Emergency Stop, trying to reconnect..."s);
	}

	void SideBarPresenter::enableBackButton(bool enable)
	{
		m_view->enableBackButton(enable);
	}

	void SideBarPresenter::enableHomeButton(bool enable)
	{
		m_view->enableHomeButton(enable);
	}

	void SideBarPresenter::onInit()
	{
		registerEventListener<EventType::NavigationBackEnable>(this, &SideBarPresenter::enableBackButton);
		registerEventListener<EventType::NavigationHomeEnable>(this, &SideBarPresenter::enableHomeButton);
	}

	void SideBarPresenter::onActivate()
	{
		enableBackButton(false);
		enableHomeButton(false);
	}
} // namespace UI

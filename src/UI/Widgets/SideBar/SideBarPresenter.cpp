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

namespace UI
{
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

	void SideBarPresenter::onActivate() {}
} // namespace UI

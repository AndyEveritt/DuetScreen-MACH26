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
#if SIDE_BAR_BACK_BUTTON
	void SideBarPresenter::enableBackButton(bool enable)
	{
		m_view->enableBackButton(enable);
	}
#endif

	void SideBarPresenter::enableHomeButton(bool enable)
	{
		m_view->enableHomeButton(enable);
	}

	void SideBarPresenter::onInit()
	{
#if SIDE_BAR_BACK_BUTTON
		registerEventListener<EventType::NavigationBackEnable>(this, &SideBarPresenter::enableBackButton);
#endif
		registerEventListener<EventType::NavigationHomeEnable>(this, &SideBarPresenter::enableHomeButton);
	}

	void SideBarPresenter::onActivate() {}
} // namespace UI

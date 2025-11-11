/*
 * Sidebar.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "EStop.h"
#include "SideBarPresenter.h"
#include "UI/Components/AppDrawer/AppDrawer.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Button/DraggableButton.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"

#define SIDE_BAR_BACK_BUTTON 1
#define SIDE_BAR_APP_DRAWER 1

namespace UI
{
	class SideBar : public View<SideBarPresenter>
	{
	  public:
		SideBar(const std::string& name, LvObj& parent);

		void enableHomeButton(bool enable);
#if SIDE_BAR_BACK_BUTTON
		void enableBackButton(bool enable);
#endif

#if SIDE_BAR_APP_DRAWER
		void showAppDrawer(bool show, bool animate = LV_ANIM_ON);
#endif

	  private:
#if SIDE_BAR_BACK_BUTTON
		static void backBtnEvent(lv_event_t* e);
#endif
		static void homeBtnEvent(lv_event_t* e);
		static void macrosBtnEvent(lv_event_t* e);
#if SIDE_BAR_APP_DRAWER
		static void menuBtnEvent(lv_event_t* e);
#endif

		void onShow() override;

		LvContainer m_btns{"buttons", getRoot()};
		Button m_homeBtn{"home", m_btns};
#if SIDE_BAR_BACK_BUTTON
		Button m_backBtn{"back", m_btns};
#endif
		Button m_menuBtn{"menu", m_btns};
		Button m_macrosBtn{"macros", m_btns};
		Button m_settingsBtn{"settings", m_btns};
		EStop m_eStopBtn{"estop", m_btns};
#if SIDE_BAR_APP_DRAWER
		AppDrawer m_appDrawer{"app_drawer", getRoot()};
#endif
		LvContainer m_appDrawerModalBg{"app_drawer_modal_bg", getRoot()};
	};
} // namespace UI

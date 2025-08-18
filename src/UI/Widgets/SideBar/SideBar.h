/*
 * Sidebar.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "SideBarPresenter.h"
#include "UI/Components/AppDrawer/AppDrawer.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Button/DraggableButton.h"
#include "UI/Core/View.h"

namespace UI
{
	class SideBar : public View<SideBarPresenter>
	{
	  public:
		SideBar(const std::string& name, lv_obj_t* parent);

		void enableHomeButton(bool enable);
		void enableBackButton(bool enable);

		void showAppDrawer(bool show, bool animate = LV_ANIM_ON);

	  private:
		static void backBtnEvent(lv_event_t* e);
		static void homeBtnEvent(lv_event_t* e);
		static void macrosBtnEvent(lv_event_t* e);
		static void menuBtnEvent(lv_event_t* e);
		static void eStopDraggedEvent(float pct, void* e);

		void onShow() override;

		LvContainer m_btns;
		Button m_homeBtn;
		Button m_backBtn;
		Button m_menuBtn;
		Button m_macrosBtn;
		DraggableButton m_eStopBtn;
		AppDrawer m_appDrawer;
	};
} // namespace UI

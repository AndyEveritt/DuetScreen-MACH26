/*
 * Sidebar.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "SideBarPresenter.h"
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

	  private:
		static void backBtnEvent(lv_event_t* e);
		static void homeBtnEvent(lv_event_t* e);
		static void macrosBtnEvent(lv_event_t* e);
		static void consoleBtnEvent(lv_event_t* e);
		static void eStopDraggedEvent(float pct, void* e);

		Button m_homeBtn;
		Button m_backBtn;
		Button m_macrosBtn;
		Button m_consoleBtn;
		DraggableButton m_eStopBtn;
	};
} // namespace UI

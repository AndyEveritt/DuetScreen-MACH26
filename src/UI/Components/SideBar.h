/*
 * Sidebar.h
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button.h"
#include "UI/Core/View.h"

namespace UI
{
	class SideBar : public BaseView
	{
	  public:
		SideBar(const std::string& name, lv_obj_t* parent);

		void setConsoleView(BaseView* view) { m_consoleView = view; }

	  private:
		static void backBtnEvent(lv_event_t* e);
		static void homeBtnEvent(lv_event_t* e);
		static void macrosBtnEvent(lv_event_t* e);
		static void consoleBtnEvent(lv_event_t* e);
		static void eStopBtnEvent(lv_event_t* e);

		Button m_backBtn;
		Button m_homeBtn;
		Button m_macrosBtn;
		Button m_consoleBtn;
		Button m_eStopBtn;

		BaseView* m_consoleView;
	};
} // namespace UI

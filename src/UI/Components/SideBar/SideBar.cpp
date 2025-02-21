/*
 * SideBar.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "SideBar.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/File/FileView.h"
#include "UI/Screens/Home/HomeView.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	static constexpr lv_coord_t width = 10;	  // %
	static constexpr lv_coord_t height = 100; // %

	SideBar::SideBar(const std::string& name, lv_obj_t* parent)
		: View(name, parent, layout_t{0, 0, width, height})
		, m_backBtn("Back", getCont(), _("back"), layout_t{0, 0, 100, 0})
		, m_homeBtn("Home", getCont(), _("home"), layout_t{0, 20, 100, 0})
		, m_macrosBtn("Macros", getCont(), _("macros"), layout_t{0, 40, 100, 0})
		, m_consoleBtn("Console", getCont(), _("console"), layout_t{0, 60, 100, 0})
		, m_eStopBtn("E-Stop", getCont(), _("estop"), layout_t{0, 80, 100, 2 * 0})
	{
		verbose("Creating SideBar");

		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_style_pad_row(getCont(), 0, 0);
		lv_obj_set_style_pad_all(getCont(), 0, 0);

		lv_obj_set_flex_grow(m_backBtn.getCont(), 1);
		lv_obj_set_flex_grow(m_homeBtn.getCont(), 1);
		lv_obj_set_flex_grow(m_macrosBtn.getCont(), 1);
		lv_obj_set_flex_grow(m_consoleBtn.getCont(), 1);
		lv_obj_set_flex_grow(m_eStopBtn.getCont(), 2);

		m_backBtn.setCallback(backBtnEvent, LV_EVENT_CLICKED, this);
		m_homeBtn.setCallback(homeBtnEvent, LV_EVENT_CLICKED, this);
		m_macrosBtn.setCallback(macrosBtnEvent, LV_EVENT_CLICKED, this);
		m_consoleBtn.setCallback(consoleBtnEvent, LV_EVENT_CLICKED, this);
		m_eStopBtn.setCallback(eStopBtnEvent, LV_EVENT_CLICKED, this);
	}

	void SideBar::backBtnEvent(lv_event_t* e)
	{
		UI::back();
	}

	void SideBar::homeBtnEvent(lv_event_t* e)
	{
		UI::home();
	}

	void SideBar::macrosBtnEvent(lv_event_t* e)
	{
		info("Macros button pressed");
		FileView& fileView = HomeView::instance().getFileView();
		fileView.getPresenter().setBaseFolder(FilePresenter::BaseFolder::MACROS);
		openScreen(&fileView, true);
	}

	void SideBar::consoleBtnEvent(lv_event_t* e)
	{
		info("Console button pressed");
		openScreen(&HomeView::instance().getConsoleView(), true);
	}

	void SideBar::eStopBtnEvent(lv_event_t* e)
	{
		info("E-Stop button pressed");
		SideBar* sb = static_cast<SideBar*>(lv_event_get_user_data(e));
		sb->m_presenter.eStop();
	}
} // namespace UI
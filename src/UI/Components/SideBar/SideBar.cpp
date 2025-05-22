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
#include "UI/Styles/Styles.h"
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
		UI_LOCK();
		LOG_VERBOSE("Creating SideBar");

		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setPad(0, LV_PART_MAIN, Padding::ALL);
		setPad(0, LV_PART_MAIN, Padding::ROW);

		m_backBtn.setFlexGrow(1);
		m_homeBtn.setFlexGrow(1);
		m_macrosBtn.setFlexGrow(1);
		m_consoleBtn.setFlexGrow(1);
		m_eStopBtn.setFlexGrow(2);

		m_backBtn.setCallback(backBtnEvent, LV_EVENT_CLICKED, this);
		m_homeBtn.setCallback(homeBtnEvent, LV_EVENT_CLICKED, this);
		m_macrosBtn.setCallback(macrosBtnEvent, LV_EVENT_CLICKED, this);
		m_consoleBtn.setCallback(consoleBtnEvent, LV_EVENT_CLICKED, this);
		m_eStopBtn.setCallback(eStopBtnEvent, LV_EVENT_CLICKED, this);

		m_eStopBtn.addStyle(Themes::getEStopStyle(), LV_PART_MAIN, true);
		// lv_obj_add_style(m_eStopBtn.getButton(), Themes::getEStopStyle(), LV_PART_MAIN, true);
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
		UI_LOCK();
		LOG_INFO("Macros button pressed");
		FileView& fileView = HomeView::instance().getFileView();
		fileView.getPresenter()->setBaseFolder(FilePresenter::BaseFolder::MACROS);
		openScreen(&fileView, true);
	}

	void SideBar::consoleBtnEvent(lv_event_t* e)
	{
		UI_LOCK();
		LOG_INFO("Console button pressed");
		openScreen(&HomeView::instance().getConsoleView(), true);
	}

	void SideBar::eStopBtnEvent(lv_event_t* e)
	{
		UI_LOCK();
		LOG_INFO("E-Stop button pressed");
		SideBar* sb = static_cast<SideBar*>(lv_event_get_user_data(e));
		sb->m_presenter->eStop();
	}
} // namespace UI
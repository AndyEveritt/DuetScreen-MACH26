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

#define ESTOP_SIZE 80

namespace UI
{
	static constexpr lv_coord_t width = 10;	  // %
	static constexpr lv_coord_t height = 100; // %

	SideBar::SideBar(const std::string& name, lv_obj_t* parent)
		: View(lv_obj_create, name, parent, layout_t{0, 0, width, height})
		, m_homeBtn("Home", getCont(), _("home"), layout_t{0, 20, 100, 0})
		, m_backBtn("Back", getCont(), _("back"), layout_t{0, 0, 100, 0})
		, m_macrosBtn("Macros", getCont(), _("macros"), layout_t{0, 40, 100, 0})
		, m_consoleBtn("Console", getCont(), _("console"), layout_t{0, 60, 100, 0})
		, m_eStopBtn("E-Stop", getCont(), _("estop"))
	{
		UI_LOCK();
		LOG_VERBOSE("Creating SideBar");

		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		lv_obj_remove_flag(getCont(), LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_overflow_visible_flag(getCont(), 400);

		m_backBtn.setFlexGrow(1);
		m_homeBtn.setFlexGrow(1);
		m_macrosBtn.setFlexGrow(1);
		m_consoleBtn.setFlexGrow(1);

		m_eStopBtn.setSize(ESTOP_SIZE, ESTOP_SIZE);

		m_backBtn.setCallback(backBtnEvent, LV_EVENT_CLICKED, this);
		m_homeBtn.setCallback(homeBtnEvent, LV_EVENT_CLICKED, this);
		m_macrosBtn.setCallback(macrosBtnEvent, LV_EVENT_CLICKED, this);
		m_consoleBtn.setCallback(consoleBtnEvent, LV_EVENT_CLICKED, this);
		m_eStopBtn.setDragCallback(eStopDraggedEvent, this);

		addStyle(Themes::getComponentStyles().sidebar, LV_PART_MAIN);
		m_eStopBtn.addStyle(Themes::getComponentStyles().estop, LV_PART_MAIN, true);
	}

	void SideBar::enableHomeButton(bool enable)
	{
		UI_LOCK();
		m_homeBtn.setInvalid(!enable);
	}

	void SideBar::enableBackButton(bool enable)
	{
		UI_LOCK();
		m_backBtn.setInvalid(!enable);
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

	void SideBar::eStopDraggedEvent(float pct, void* sidebar)
	{
		UI_LOCK();
		LOG_INFO("E-Stop button dragged");
		SideBar* sb = static_cast<SideBar*>(sidebar);
		if (pct < 0.5f)
		{
			Model::get().post<EventType::Response>(std::string(_("estop_prompt")));
		}

		if (pct == 1.0f)
		{
			sb->m_presenter->eStop();
		}
	}
} // namespace UI
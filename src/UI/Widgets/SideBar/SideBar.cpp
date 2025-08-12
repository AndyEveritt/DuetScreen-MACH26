/*
 * SideBar.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "SideBar.h"
#include "Debug.h"
#include "UI/Components/LVGL/LvAnim.h"
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
		: View(name, parent, layout_t{0, 0, width, height})
		, m_btns("buttons", getRoot())
		, m_homeBtn("home", m_btns, _("home"))
		, m_backBtn("back", m_btns, _("back"))
		, m_menuBtn("menu", m_btns, _("menu"))
		, m_macrosBtn("macros", m_btns, _("macros"))
		, m_eStopBtn("estop", m_btns, _("estop"))
		, m_appDrawer("app_drawer", getRoot())
	{
		LOG_VERBOSE("Creating SideBar");

		setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		m_btns.setSize(LV_PCT(100), LV_PCT(100));
		m_btns.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_btns.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_btns.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		m_backBtn.setWidth(LV_PCT(100));
		m_homeBtn.setWidth(LV_PCT(100));
		m_macrosBtn.setWidth(LV_PCT(100));
		m_menuBtn.setWidth(LV_PCT(100));

		setExtDrawSize(400);
		m_btns.setExtDrawSize(400);

		m_backBtn.setFlexGrow(1);
		m_homeBtn.setFlexGrow(1);
		m_macrosBtn.setFlexGrow(1);
		m_menuBtn.setFlexGrow(1);

		m_eStopBtn.setSize(ESTOP_SIZE, ESTOP_SIZE);

		m_backBtn.addClickedCallback(backBtnEvent, this);
		m_homeBtn.addClickedCallback(homeBtnEvent, this);
		m_macrosBtn.addClickedCallback(macrosBtnEvent, this);
		m_menuBtn.addClickedCallback(menuBtnEvent, this);
		m_eStopBtn.setDragCallback(eStopDraggedEvent, this);

		// m_appDrawer.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_appDrawer.setSize(LV_SIZE_CONTENT, LV_PCT(100));
		m_appDrawer.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_appDrawer.hide(true);

		addStyle(Themes::getComponentStyles().sidebar, LV_PART_MAIN);
		addStyle(Themes::getLvglStyles().pad_zero);
		m_btns.addStyle(Themes::getLvglStyles().bg_dark);
		m_eStopBtn.addStyle(Themes::getComponentStyles().estop, LV_PART_MAIN, true);
	}

	void SideBar::enableHomeButton(bool enable)
	{
		m_homeBtn.setDisabled(!enable);
	}

	void SideBar::enableBackButton(bool enable)
	{
		m_backBtn.setDisabled(!enable);
		showAppDrawer(false);
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
		LOG_INFO("Macros button pressed");
		FileView& macrosView = HomeView::instance().getMacroView();
		macrosView.getPresenter()->setBaseFolder(FilePresenter::BaseFolder::MACROS);
		openScreen(&macrosView, true);
	}

	void SideBar::menuBtnEvent(lv_event_t* e)
	{
		LOG_DBG("Menu button pressed");
		SideBar& sidebar = *static_cast<SideBar*>(lv_event_get_user_data(e));

		sidebar.showAppDrawer(sidebar.m_appDrawer.hasFlag(LV_OBJ_FLAG_HIDDEN));
	}

	void SideBar::eStopDraggedEvent(float pct, void* sidebar)
	{
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

	void SideBar::showAppDrawer(bool show)
	{
		LvAnim anim;
		if (show == !m_appDrawer.hasFlag(LV_OBJ_FLAG_HIDDEN))
		{
			LOG_DBG("App drawer is already {}", show ? "shown" : "hidden");
			return;
		}
		anim.setDuration(300);
		anim.setVar(&m_appDrawer);
		int32_t start = show ? 0 : m_appDrawer.getWidth();
		int32_t end = show ? m_appDrawer.getWidth() : 0;
		anim.setValues(start, end);
		anim.setExecCb(
			[](void* var, int32_t value)
			{
				auto& drawer = *static_cast<AppDrawer*>(var);
				drawer.setX(value);
			});
		anim.setCompletedCb(
			[](lv_anim_t* anim)
			{
				auto& drawer = *static_cast<AppDrawer*>(anim->var);
				drawer.setFlag(LV_OBJ_FLAG_HIDDEN, drawer.getCoords().x1 <= 0);
			});

		if (show)
			m_appDrawer.setFlag(LV_OBJ_FLAG_HIDDEN, false);

		anim.start();
	}

	void SideBar::onShow()
	{
		m_appDrawer.init();
	}
} // namespace UI
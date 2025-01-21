#pragma once

#include "HomePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/Graph/Graph.h"
#include "UI/Components/List.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Components/SideBar.h"
#include "UI/Components/ToolList/ToolList.h"
#include "UI/Core/View.h"
#include "UI/Screens/Settings/SettingsView.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class HomeView : public View<HomePresenter>
	{
	  public:
		friend class HomePresenter;

		static HomeView& instance()
		{
			static HomeView view;
			return view;
		}

	  private:
		HomeView();
		virtual void onShow() {}
		virtual void onHide();

		static void onWindowSelectEvent(lv_event_t* e);

		SideBar m_sideBar;
		lv_obj_t* m_mainWindow;
		ToolList m_toolList;
		Graph m_graph;

		// Window selector
		lv_obj_t* m_windowSelect;
		Button m_moveWindow;
		Button m_extrudeWindow;
		Button m_statusWindow;
		Button m_heightmapWindow;
		Button m_fansWindow;
		Button m_objectCancelWindow;
		Button m_filesWindow;
		Button m_settingsWindow;

		// Windows
		SettingsView m_settingsView;
	};
} // namespace UI
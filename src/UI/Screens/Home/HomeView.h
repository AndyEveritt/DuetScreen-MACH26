#pragma once

#include "HomePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/Graph/Graph.h"
#include "UI/Components/List.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Components/SideBar.h"
#include "UI/Components/ToolList/ToolList.h"
#include "UI/Core/View.h"
#include "UI/Screens/Sub/SubView.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class HomeView : public View<HomePresenter>
	{
	  public:
		HomeView();

	  private:
		virtual void onShow() {}
		virtual void onHide();

		SideBar m_sideBar;
		lv_obj_t* m_mainWindow;
		ToolList m_toolList;
		Graph m_graph;
	};
} // namespace UI
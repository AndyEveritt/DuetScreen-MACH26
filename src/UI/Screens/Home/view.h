#pragma once

#include "UI/Core/view.h"
#include "lvgl/lvgl.h"
#include "presenter.h"

namespace UI
{
	class HomeView : public View<HomePresenter>
	{
	  public:
		HomeView();

	  private:
		lv_obj_t* tabview;
		lv_obj_t* main_tab;
		lv_obj_t* macros_tab;
	};
} // namespace UI
#pragma once

#include "UI/Core/view.h"
#include "UI/Screens/Sub/view.h"
#include "lvgl/lvgl.h"
#include "presenter.h"

namespace UI
{
	class HomeView : public View<HomePresenter>
	{
	  public:
		HomeView();

		void setHeaterData(const char* data);

	  private:
		virtual void onShow() { m_subView.show(); }
		virtual void onHide() { m_subView.hide(); }

		lv_obj_t* tabview;
		lv_obj_t* main_tab;
		lv_obj_t* macros_tab;
		lv_obj_t* m_label;
		SubView m_subView;
	};
} // namespace UI
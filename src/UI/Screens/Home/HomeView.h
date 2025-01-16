#pragma once

#include "HomePresenter.h"
#include "UI/Components/button.h"
#include "UI/Components/list.h"
#include "UI/Core/view.h"
#include "UI/Screens/Sub/view.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class HomeView : public View<HomePresenter>
	{
	  public:
		HomeView();

		void setHeaterData(const char* data);

		void openSubView();

	  private:
		virtual void onShow() {}
		virtual void onHide() {}

		static void btnCallback(lv_event_t* e);
		static void btn2Callback(lv_event_t* e);

		lv_obj_t* tabview;
		lv_obj_t* main_tab;
		lv_obj_t* macros_tab;
		lv_obj_t* m_label;
		SubView m_subView;
		Button m_btn;
		Button m_btn2;
		DynamicList m_list;
	};
} // namespace UI
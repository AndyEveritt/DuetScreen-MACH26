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

		virtual void onShow() { m_subView.show(); }

		void setHeaterData(const char* data)
		{
			lv_lock();
			lv_label_set_text(m_label, data);
			lv_unlock();
		}

	  private:
		lv_obj_t* tabview;
		lv_obj_t* main_tab;
		lv_obj_t* macros_tab;
		lv_obj_t* m_label;
		SubView m_subView;
	};
} // namespace UI
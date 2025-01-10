#pragma once

#include "UI/Core/view.h"
#include "presenter.h"

namespace UI
{
	class SubView : public View<SubPresenter>
	{
	  public:
		SubView(lv_obj_t* parent);

	  private:
		lv_obj_t* tabview;
		lv_obj_t* main_tab;
		lv_obj_t* macros_tab;
	};
} // namespace UI
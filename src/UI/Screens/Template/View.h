#pragma once

#include "Presenter.h"
#include "UI/Core/View.h"

namespace UI
{
	class CustomView : public View<CustomPresenter>
	{
	  public:
		CustomView(lv_obj_t* parent);

	  private:
	};
} // namespace UI
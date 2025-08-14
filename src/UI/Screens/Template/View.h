#pragma once

#include "Presenter.h"
#include "UI/Core/View.h"

namespace UI
{
	class CustomView : public View<CustomPresenter>
	{
	  public:
		CustomView(const std::string& name, lv_obj_t* parent);

	  private:
	};
} // namespace UI
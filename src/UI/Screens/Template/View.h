#pragma once

#include "CustomPresenter.h"
#include "UI/Core/View.h"

namespace UI
{
	class CustomView : public View<CustomPresenter>
	{
	  public:
		CustomView(const std::string& name, LvObj& parent);

	  private:
	};
} // namespace UI
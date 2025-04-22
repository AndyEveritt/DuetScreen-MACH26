#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class CustomView;

	class CustomPresenter : public Presenter<CustomView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(CustomPresenter, CustomView)

		// Actions

		// Observers

	  private:
	};
} // namespace UI

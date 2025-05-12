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

	  protected:
		virtual void onInit() override {}
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}
	};
} // namespace UI

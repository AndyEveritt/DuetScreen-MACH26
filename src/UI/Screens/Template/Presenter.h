#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class CustomView;

	class CustomPresenter : public Presenter<CustomView>
	{
	  public:
		using Presenter::Presenter;

		// Actions

		// Observers

	  private:
	};
} // namespace UI

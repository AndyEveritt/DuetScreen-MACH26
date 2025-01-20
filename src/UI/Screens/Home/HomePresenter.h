#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class HomeView;

	class HomePresenter : public Presenter<HomeView>
	{
	  public:
		using Presenter::Presenter;

		void tick();

	  protected:
	};
} // namespace UI

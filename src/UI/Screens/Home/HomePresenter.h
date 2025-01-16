#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class HomeView;

	class HomePresenter : public Presenter<HomeView>
	{
	  public:
		using Presenter::Presenter;

		virtual void newHeaterData() override;

	  protected:
	};
} // namespace UI

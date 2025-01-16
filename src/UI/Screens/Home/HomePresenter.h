#pragma once

#include "UI/Core/presenter.h"

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

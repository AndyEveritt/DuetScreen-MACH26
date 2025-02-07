#pragma once

#include "ObjectModel/Axis.h"
#include "UI/Core/Presenter.h"
#include <vector>

namespace UI
{
	class HomeView;

	class HomePresenter : public Presenter<HomeView>
	{
	  public:
		using Presenter::Presenter;
		// PRESENTER_CONSTRUCTOR(HomePresenter)
		void init();

		void tick() override;
		virtual void refresh() override;

		virtual void newAxesData() override;
		virtual void newResponse(const char* response) override;
		virtual void newMessageBoxData(const OM::Alert& alert) override;

	  protected:
		std::vector<char> m_alertAxes;
	};
} // namespace UI

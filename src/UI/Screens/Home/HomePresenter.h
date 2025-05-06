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
		// using Presenter::Presenter;
		PRESENTER_CONSTRUCTOR(HomePresenter, HomeView)

		virtual void init() override;

		void tick() override;
		virtual void disconnected() override;

		void update();

		virtual void newUpdateAvailable(const std::string& file) override;
		virtual void newAxesData() override;
		virtual void newResponse(const std::string& response) override;
		virtual void newMessageBoxData(const OM::Alert& alert) override;

	  protected:
		std::vector<char> m_alertAxes;
		std::string m_updateFile;
	};
} // namespace UI

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

		void tick();
		void disconnected();

		void update();

		void newUpdateAvailable(const std::string& file);
		void newAxesData();
		void newResponse(const std::string& response);
		void newMessageBoxData(const OM::Alert& alert);

	  protected:
		virtual void onInit() override;
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}

		std::vector<char> m_alertAxes;
		std::string m_updateFile;
	};
} // namespace UI

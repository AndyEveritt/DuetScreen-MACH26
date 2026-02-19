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

		void clear();

		void update();

		void newUpdateAvailable(const std::string& file);
		void newAxesData();
		void newResponse(const ResponseType type, const std::string& response);
		void newAlertData(const OM::Alert& alert);
		void newStatus(OM::PrinterStatus status);

	  protected:
		void onInit() override;
		void onActivate() override;
		void onDeactivate() override {}
		void onDisconnect() override { clear(); }

		std::vector<char> m_alertAxes;
		std::string m_updateFile;
	};
} // namespace UI

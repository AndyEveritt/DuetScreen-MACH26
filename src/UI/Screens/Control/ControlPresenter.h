#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ControlView;

	class ControlPresenter : public Presenter<ControlView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ControlPresenter, ControlView);

		// Setters

		// Getters

		// Actions

		// Observers

	  protected:
		void onInit() override {}
		void onActivate() override {}
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}
	};
} // namespace UI

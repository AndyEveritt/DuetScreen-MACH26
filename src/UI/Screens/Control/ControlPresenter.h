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
		virtual void onInit() override {}
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() override {}
	};
} // namespace UI

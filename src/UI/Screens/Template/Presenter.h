#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class CustomView;

	class CustomPresenter : public Presenter<CustomView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(CustomPresenter, CustomView);

		// Setters

		// Getters

		// Actions

		// Observers

	  protected:
		void onInit() override {}
		void onActivate() override {}
		void onDeactivate() override {}

		virtual void onConnect() {}
		virtual void onDisconnect() {}
	};
} // namespace UI

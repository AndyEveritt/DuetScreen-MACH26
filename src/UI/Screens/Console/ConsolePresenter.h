#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ConsoleView;

	class ConsolePresenter : public Presenter<ConsoleView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		bool back();
		void sendGcode(const char* gcode);

		// Observers
		void newResponse(const char* resp) override;

	  private:
		void onActivate() override;
	};
} // namespace UI

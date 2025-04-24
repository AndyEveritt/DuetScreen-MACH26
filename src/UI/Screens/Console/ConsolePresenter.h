#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ConsoleView;

	class ConsolePresenter : public Presenter<ConsoleView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ConsolePresenter, ConsoleView)

		// Actions
		bool back();
		void sendGcode(const char* gcode);

		// Observers
		void newResponse(const char* resp) override;
		void newLogMessage(const Log::DebugLevel& level,
						   const std::chrono::_V2::system_clock::time_point& time,
						   const std::string& message) override;

	  private:
		void onActivate() override;
	};
} // namespace UI

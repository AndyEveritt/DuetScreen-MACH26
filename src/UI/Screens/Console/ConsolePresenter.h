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
		void sendCommand(std::string_view cmd);

#if ENABLE_CONSOLE_SHELL
		void enableShell(bool enable) { m_shellEnabled = enable; }
#endif

		// Observers
		void newResponse(ResponseType type, const std::string& resp);
		void newLogMessage(const Log::DebugLevel& level, const Log::log_time_t& time, const std::string& message);

	  private:
		void onActivate() override;

		virtual void onInit() override
		{
			registerEventListener<EventType::Response>(this, &ConsolePresenter::newResponse);
			registerEventListener<EventType::LogMessage>(this, &ConsolePresenter::newLogMessage);
		}

#if ENABLE_CONSOLE_SHELL
		bool m_shellEnabled = false;
#endif
	};
} // namespace UI

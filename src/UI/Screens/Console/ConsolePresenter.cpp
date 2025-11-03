#include "ConsolePresenter.h"
#include "ConsoleView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "i18n/i18n.h"

namespace UI
{
	bool ConsolePresenter::back()
	{
		return false;
	}

	void ConsolePresenter::newResponse(const std::string& resp)
	{
		m_view->addResponse(resp);
	}

	void ConsolePresenter::newLogMessage(const Log::DebugLevel& level,
										 const Log::log_time_t& time,
										 const std::string& message)
	{
		m_view->addResponse(message);
	}

	void ConsolePresenter::sendCommand(std::string_view cmd)
	{
		m_view->addCommand(cmd);
#if ENABLE_CONSOLE_SHELL
		if (m_shellEnabled)
		{
			std::string result;

			FILE* pipe = ::popen(cmd.data(), "r");
			if (!pipe)
				return;
			char buffer[256];
			while (fgets(buffer, sizeof(buffer), pipe))
			{
				result.append(buffer);
			}
			::pclose(pipe);

			m_view->addResponse(result);
		}
		else
		{
#endif
			Comm::DUET.SendGcode(cmd);
#if ENABLE_CONSOLE_SHELL
		}
#endif
	}

	void ConsolePresenter::onActivate() {}
} // namespace UI

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

	void ConsolePresenter::sendGcode(std::string_view gcode)
	{
		m_view->addCommand(gcode);
		Comm::DUET.SendGcode(gcode);
	}

	void ConsolePresenter::onActivate() {}
} // namespace UI

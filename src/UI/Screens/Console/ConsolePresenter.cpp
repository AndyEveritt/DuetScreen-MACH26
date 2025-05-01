#include "ConsolePresenter.h"
#include "ConsoleView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	bool ConsolePresenter::back()
	{
		return false;
	}

	void ConsolePresenter::newResponse(const std::string& resp)
	{
		m_view->addResponse(resp.c_str());
	}

	void ConsolePresenter::newLogMessage(const Log::DebugLevel& level,
										 const Log::log_time_t& time,
										 const std::string& message)
	{
		m_view->addResponse(message.c_str());
	}

	void ConsolePresenter::sendGcode(const char* gcode)
	{
		Comm::DUET.SendGcode(gcode);
	}

	void ConsolePresenter::onActivate() {}
} // namespace UI

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

	void ConsolePresenter::newResponse(const char* resp)
	{
		m_view->addResponse(resp);
		m_buffer.Push(resp);
		m_view->refresh();
	}

	void ConsolePresenter::onActivate() {}
} // namespace UI

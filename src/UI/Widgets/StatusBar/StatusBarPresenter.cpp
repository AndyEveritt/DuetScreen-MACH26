#include "StatusBarPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "StatusBar.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	void StatusBarPresenter::onActivate()
	{
		newDuetName();
		newStatus(OM::GetStatus());
		newTime();
	}

	void StatusBarPresenter::onConnect()
	{
		m_view->setDuetStatus(
			fmt::format("{:s} - {:s}", _(Comm::DUET.GetCommunicationTypeName().data()), _("connected")));
	}

	void StatusBarPresenter::onDisconnect()
	{
		m_view->setDuetStatus(
			fmt::format("{:s} - {:s}", _(Comm::DUET.GetCommunicationTypeName().data()), _("disconnected")));
		m_view->setDuetName("");
	}

	void StatusBarPresenter::newDuetName()
	{
		m_view->setDuetName(OM::GetPrinterName());
	}

	void StatusBarPresenter::newStatus(const OM::PrinterStatus& status)
	{
		m_view->setDuetStatus(
			fmt::format("{:s} - {:s}", _(Comm::DUET.GetCommunicationTypeName().data()), _(OM::GetStatusText())));
	}

	void StatusBarPresenter::newTime()
	{
		m_view->setTime("");
		// m_view->setTime(TimeHelper::getCurrentTimeString());
	}
} // namespace UI

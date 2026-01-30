#include "StatusBarPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/PrinterStatus.h"
#include "StatusBar.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	void StatusBarPresenter::onActivate()
	{
		ZoneScoped;
		newDuetName();
		newStatus(OM::GetStatus());
		newTime();
	}

	void StatusBarPresenter::onConnect()
	{
		ZoneScoped;
		m_view->setDuetStatus(
			fmt::format("{:s} - {:s}", _(Comm::DUET.GetCommunicationTypeName()), _("state.connected")));
	}

	void StatusBarPresenter::onDisconnect()
	{
		ZoneScoped;
		m_view->setDuetStatus(
			fmt::format("{:s} - {:s}", _(Comm::DUET.GetCommunicationTypeName()), _("state.disconnected")));
		m_view->setDuetName("");
	}

	void StatusBarPresenter::newDuetName()
	{
		ZoneScoped;
		m_view->setDuetName(OM::GetPrinterName());
	}

	void StatusBarPresenter::newStatus(const OM::PrinterStatus& /* status */)
	{
		ZoneScoped;
		m_view->setDuetStatus(fmt::format("{:s} - {:s}",
										  _(Comm::DUET.GetCommunicationTypeName()),
										  _(fmt::format("state.{:s}", OM::GetStatusText()))));
	}

	void StatusBarPresenter::newTime()
	{
		ZoneScoped;
		m_view->setTime("");
		// m_view->setTime(TimeHelper::getCurrentTimeString());
	}
} // namespace UI

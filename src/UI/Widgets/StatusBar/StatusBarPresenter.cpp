#include "StatusBarPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Heat.h"
#include "ObjectModel/PrinterStatus.h"
#include "ObjectModel/Tool.h"
#include "StatusBar.h"
#include "UI/Styles/Styles.h"
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
		newHeaterData();
	}

	void StatusBarPresenter::onConnect()
	{
		ZoneScoped;
		getView()->setDuetStatus(
			fmt::format("{:s} - {:s}", _(Comm::DUET.GetCommunicationTypeName()), _("state.connected")));
	}

	void StatusBarPresenter::onDisconnect()
	{
		ZoneScoped;
		getView()->setDuetStatus(
			fmt::format("{:s} - {:s}", _(Comm::DUET.GetCommunicationTypeName()), _("state.disconnected")));
		getView()->setDuetName("");
		newHeaterData();
	}

	void StatusBarPresenter::newDuetName()
	{
		ZoneScoped;
		getView()->setDuetName(OM::GetPrinterName());
	}

	void StatusBarPresenter::newStatus(const OM::PrinterStatus& /* status */)
	{
		ZoneScoped;
		getView()->setDuetStatus(fmt::format("{:s} - {:s}",
											 _(Comm::DUET.GetCommunicationTypeName()),
											 _(fmt::format("state.{:s}", OM::GetStatusText()))));
	}

	void StatusBarPresenter::newTime()
	{
		ZoneScoped;
		getView()->setTime("");
		// getView()->setTime(TimeHelper::getCurrentTimeString());
	}

	void StatusBarPresenter::newHeaterData()
	{
		ZoneScoped;

		std::string temperatureStr;
		auto& spangroup = getView()->getTemperatures();

		const auto currentTool = OM::GetCurrentTool();
		size_t spans = 0;
		if (currentTool && currentTool->GetHeaterCount() > 0)
		{
			auto toolSpan = spangroup.getOrCreateSpanByIndex(spans);
			spans++;
			toolSpan.setText(fmt::format("T{:d}:", currentTool->index));
			toolSpan.setStyleStatic(Themes::getLvglStyles().text_emphasis);

			currentTool->IterateHeaters(
				[&](OM::ToolHeaterPtr th, size_t /* index */)
				{
					if (const auto heater = th->heater)
					{
						auto span = spangroup.getOrCreateSpanByIndex(spans);
						spans++;
						span.setText(fmt::format(" {:g}°C/{:d}°C", heater->current, heater->GetTargetTemp()));
						span.setStyleStatic(heater->status == OM::Heat::HeaterStatus::fault
												? Themes::getLvglStyles().text_warning
												: Themes::getLvglStyles().text);
					}
				});
		}

		spangroup.removeSpansFromIndex(spans, true);
	}
} // namespace UI

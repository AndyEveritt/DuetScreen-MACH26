#include "ExtrudePresenter.h"
#include "Debug.h"
#include "ExtrudeView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void ExtrudePresenter::retract(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcodef("G1 E-%u F%u\n", distance, feedrate * 60);
	}

	void ExtrudePresenter::extrude(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcodef("G1 E%u F%u\n", distance, feedrate * 60);
	}

	void ExtrudePresenter::newToolData()
	{
		MODEL_LOCK();
		m_view->setToolCount(OM::GetToolCount());
		for (size_t i = 0; i < m_view->getToolCount(); i++)
		{
			auto item = m_view->getExtruderItem(i);
			if (item == nullptr)
			{
				continue;
			}
			OM::Tool* tool = OM::GetToolBySlot(i);
			if (tool == nullptr)
			{
				continue;
			}

			std::string toolName = tool->name.IsEmpty()
									   ? utils::format("%s %d", _("default_tool_name"), tool->index).c_str()
									   : tool->name.c_str();
			item->setLabel(toolName.c_str());
			item->setHeaterCount(tool->GetHeaterCount());
			item->setSelected(tool->status == OM::ToolStatus::active);
			tool->IterateHeaters(
				[&item](OM::ToolHeater* th, size_t index)
				{
					item->setActiveTemperature(index, th->activeTemp);
					item->setStandbyTemperature(index, th->standbyTemp);
					OM::Heat::Heater* heater = th->heater;
					if (heater == nullptr)
					{
						warn("Heater is null");
						return;
					}
					item->setHeaterName(index, heater->GetName());
					item->setStatus(index, heater->GetHeaterStatusStr());
					item->setCurrentTemperature(index, heater->current);
				});
			item->showFilamentControls(tool->filamentExtruder >= 0);
			item->setFilamentOptions(m_filamentOptions);
			item->setLoadedFilament(tool->GetFilament().c_str());
		}
	}

	void ExtrudePresenter::updateFilamentList()
	{
		for (size_t i = 0; i < m_view->getToolCount(); i++)
		{
			auto item = m_view->getExtruderItem(i);
			if (item == nullptr)
			{
				continue;
			}
			item->setFilamentOptions(m_filamentOptions);
		}
	}

	void ExtrudePresenter::toggleToolState(size_t index)
	{
		MODEL_LOCK();
		OM::Tool* tool = OM::GetToolBySlot(index);
		if (tool == nullptr)
		{
			return;
		}
		tool->ToggleState();
	}

	void ExtrudePresenter::toggleHeaterState(size_t toolIndex, size_t heaterIndex)
	{
		MODEL_LOCK();
		OM::Tool* tool = OM::GetToolBySlot(toolIndex);
		if (tool == nullptr)
		{
			return;
		}
		tool->ToggleHeaterState(heaterIndex);
	}

	void ExtrudePresenter::loadFilament(size_t index, const char* filament)
	{
		MODEL_LOCK();
		OM::Tool* tool = OM::GetToolBySlot(index);
		if (tool == nullptr)
		{
			return;
		}
		tool->ChangeFilament(filament);
	}

	void ExtrudePresenter::unloadFilament(size_t index)
	{
		MODEL_LOCK();
		OM::Tool* tool = OM::GetToolBySlot(index);
		if (tool == nullptr)
		{
			return;
		}
		tool->UnloadFilament();
	}

	bool ExtrudePresenter::configureNumberPad(const size_t toolIndex, const size_t heaterIndex, const bool active)
	{
		MODEL_LOCK();
		OM::Tool* tool = OM::GetToolBySlot(toolIndex);
		if (tool == nullptr)
		{
			error("Failed to get tool %d", toolIndex);
			return false;
		}
		OM::ToolHeater* th = tool->GetHeater(heaterIndex);
		if (th == nullptr)
		{
			error("Failed to get tool %d tHeater %d", toolIndex, heaterIndex);
			return false;
		}
		OM::Heat::Heater* heater = th->heater;
		if (heater == nullptr)
		{
			error("Failed to get tool %d heater %d", toolIndex, heaterIndex);
			return false;
		}

		ToolListNumPad& np = m_view->m_numberPad;
		std::string header = utils::format(
			_("tool_list_numpad_header_tool_heater"), tool->index, th->index, active ? _("active") : _("standby"));

		np.setHeader(header.c_str());
		np.setMinValue(heater->min);
		np.setMaxValue(heater->max);
		m_numberPadData = {toolIndex, heaterIndex, active};
		np.setConfirmCallback(numberPadConfirmCallback, this);
		return true;
	}

	void ExtrudePresenter::numberPadConfirmCallback(lv_event_t* e)
	{
		auto presenter = (ExtrudePresenter*)lv_event_get_user_data(e);
		NumberPad* np = (NumberPad*)lv_event_get_param(e);
		OM::Tool* tool = OM::GetToolBySlot(presenter->m_numberPadData.toolIndex);

		if (tool == nullptr)
		{
			error("Tool is null");
			return;
		}
		tool->SetHeaterTemps(presenter->m_numberPadData.heaterIndex, np->getValue(), presenter->m_numberPadData.active);
	}

	void ExtrudePresenter::onActivate()
	{
		MODEL_LOCK();
		OM::FileSystem::RequestFiles("/filaments",
									 [this]()
									 {
										 MODEL_LOCK();
										 this->m_filamentOptions.clear();
										 for (size_t i = 0; i < OM::FileSystem::GetItemCount(); i++)
										 {
											 std::shared_ptr<OM::FileSystem::FileSystemItem> item =
												 OM::FileSystem::GetItem(i);
											 if (item == nullptr)
											 {
												 continue;
											 }
											 this->m_filamentOptions.push_back(item->GetName());
										 }
										 this->updateFilamentList();
									 });
		newToolData();
	}
} // namespace UI

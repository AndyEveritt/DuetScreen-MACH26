#include "TemperaturePresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "TemperatureView.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void TemperaturePresenter::retract(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcodef("G1 E-%u F%u\n", distance, feedrate * 60);
	}

	void TemperaturePresenter::extrude(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcodef("G1 E%u F%u\n", distance, feedrate * 60);
	}

	void TemperaturePresenter::newToolData()
	{
		m_view->setToolCount(OM::GetToolCount());
		for (size_t i = 0; i < m_view->getToolCount(); i++)
		{
			auto item = m_view->getExtruderItem(i);
			if (item == nullptr)
			{
				continue;
			}
			auto tool = OM::GetToolBySlot(i);
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
				[&item](std::shared_ptr<OM::ToolHeater> th, size_t index)
				{
					item->setActiveTemperature(index, th->activeTemp);
					item->setStandbyTemperature(index, th->standbyTemp);
					auto heater = th->heater;
					if (heater == nullptr)
					{
						LOG_WARN("Heater is null");
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

	void TemperaturePresenter::disconnected()
	{
		m_view->setToolCount(0);
	}

	void TemperaturePresenter::updateFilamentList()
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

	void TemperaturePresenter::toggleToolState(size_t index)
	{
		MODEL_LOCK();
		auto tool = OM::GetToolBySlot(index);
		if (tool == nullptr)
		{
			return;
		}
		tool->ToggleState();
	}

	void TemperaturePresenter::toggleHeaterState(size_t toolIndex, size_t heaterIndex)
	{
		MODEL_LOCK();
		auto tool = OM::GetToolBySlot(toolIndex);
		if (tool == nullptr)
		{
			return;
		}
		tool->ToggleHeaterState(heaterIndex);
	}

	void TemperaturePresenter::loadFilament(size_t index, const char* filament)
	{
		MODEL_LOCK();
		auto tool = OM::GetToolBySlot(index);
		if (tool == nullptr)
		{
			return;
		}
		tool->ChangeFilament(filament);
	}

	void TemperaturePresenter::unloadFilament(size_t index)
	{
		MODEL_LOCK();
		auto tool = OM::GetToolBySlot(index);
		if (tool == nullptr)
		{
			return;
		}
		tool->UnloadFilament();
	}

	bool TemperaturePresenter::configureNumberPad(const size_t toolIndex, const size_t heaterIndex, const bool active)
	{
		MODEL_LOCK();
		auto tool = OM::GetToolBySlot(toolIndex);
		if (tool == nullptr)
		{
			LOG_ERROR("Failed to get tool {:d}", toolIndex);
			return false;
		}
		auto th = tool->GetHeater(heaterIndex);
		if (th == nullptr)
		{
			LOG_ERROR("Failed to get tool {:d} tHeater {:d}", toolIndex, heaterIndex);
			return false;
		}
		auto heater = th->heater;
		if (heater == nullptr)
		{
			LOG_ERROR("Failed to get tool {:d} heater {:d}", toolIndex, heaterIndex);
			return false;
		}

		ToolListNumPad& np = m_view->m_numberPad;
		std::string header = utils::format(
			_("tool_list_numpad_header_tool_heater"), tool->index, th->index, active ? _("active") : _("standby"));

		np.setHeader(header.c_str());
		np.setMinValue(heater->min);
		np.setMaxValue(heater->max);
		m_numberPadData = {toolIndex, heaterIndex, active};
		np.setConfirmCallback([this](float value) { numberPadConfirmCallback(value); });
		return true;
	}

	void TemperaturePresenter::numberPadConfirmCallback(float value)
	{
		auto tool = OM::GetToolBySlot(m_numberPadData.toolIndex);

		if (tool == nullptr)
		{
			LOG_ERROR("Tool is null");
			return;
		}
		tool->SetHeaterTemps(m_numberPadData.heaterIndex, value, m_numberPadData.active);
	}

	void TemperaturePresenter::onActivate()
	{
		MODEL_LOCK();
		OM::FileSystem::RequestFiles(OM::Directories::DirectoryType::FILAMENTS,
									 "",
									 [this]()
									 {
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
										 }
										 this->updateFilamentList();
									 });
		newToolData();
	}
} // namespace UI

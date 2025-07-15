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
		MODEL_LOCK();
		auto& tools = m_view->getTools();
		std::vector<size_t> toolIndices;
		toolIndices.reserve(OM::GetToolCount());
		OM::IterateToolsWhile(
			[&toolIndices](OM::ToolPtr tool, size_t index)
			{
				if (tool->GetHeaterCount() > 0)
				{
					toolIndices.push_back(index);
				}
				return true;
			});

		tools.setItemCount(toolIndices.size(),
						   [this, &toolIndices](size_t index, lv_obj_t* parent) -> std::shared_ptr<ToolControl>
						   {
							   auto control = std::make_shared<ToolControl>(
								   fmt::format("{}_tool_{}", m_view->getName(), index), parent);
							   control->getPresenter()->setToolIndex(toolIndices[index]);
							   control->setNumberPad(&m_view->getNumberPad());
							   control->activate();
							   return control;
						   });
	}

	void TemperaturePresenter::newBedHeaterData()
	{
		auto& beds = m_view->getBeds();
		beds.clear();
		beds.setItemCount(OM::GetBedCount(),
						  [this](size_t index, lv_obj_t* parent) -> std::shared_ptr<HeaterSlider>
						  {
							  auto control = std::make_shared<HeaterSlider>(
								  fmt::format("{}_bed_{}", m_view->getName(), index), parent);
							  control->getPresenter()->setBedIndex(index);
							  control->setNumberPad(&m_view->getNumberPad());
							  control->activate();
							  return control;
						  });
	}

	void TemperaturePresenter::newChamberHeaterData()
	{
		auto& chambers = m_view->getChambers();
		chambers.clear();
		chambers.setItemCount(OM::GetChamberCount(),
							  [this](size_t index, lv_obj_t* parent) -> std::shared_ptr<HeaterSlider>
							  {
								  auto control = std::make_shared<HeaterSlider>(
									  fmt::format("{}_chamber_{}", m_view->getName(), index), parent);
								  control->getPresenter()->setChamberIndex(index);
								  control->setNumberPad(&m_view->getNumberPad());
								  control->activate();
								  return control;
							  });
	}

	void TemperaturePresenter::onDisconnect()
	{
		m_view->getTools().clear();
		m_view->getBeds().clear();
		m_view->getChambers().clear();
	}

	void TemperaturePresenter::onActivate()
	{
		MODEL_LOCK();

		newToolData();
		newBedHeaterData();
		newChamberHeaterData();

		for (auto& tool : m_view->getTools())
		{
			tool->activate();
		}
		for (auto& bed : m_view->getBeds())
		{
			bed->activate();
		}
		for (auto& chamber : m_view->getChambers())
		{
			chamber->activate();
		}

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
										 //  this->updateFilamentList();
									 });
	}

	void TemperaturePresenter::onDeactivate()
	{
		MODEL_LOCK();

		for (auto& tool : m_view->getTools())
		{
			tool->deactivate();
		}
		for (auto& bed : m_view->getBeds())
		{
			bed->deactivate();
		}
		for (auto& chamber : m_view->getChambers())
		{
			chamber->deactivate();
		}
	}
} // namespace UI

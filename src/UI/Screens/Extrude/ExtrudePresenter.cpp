#include "ExtrudePresenter.h"
#include "Debug.h"
#include "ExtrudeView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"

namespace UI
{
	void ExtrudePresenter::retract(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcode("G28\n");
	}

	void ExtrudePresenter::extrude(uint32_t distance, uint32_t feedrate)
	{
		Comm::DUET.SendGcode("G32\n");
	}

	void ExtrudePresenter::newToolData()
	{
		ModelLock lock;
		m_view->setToolCount(OM::GetToolCount());
		for (size_t i = 0; i < m_view->getToolCount(); i++)
		{
			auto item = m_view->getExtruderItem(i);
			if (item == nullptr)
			{
				continue;
			}
			OM::Tool* tool = OM::GetTool(i);
			if (tool == nullptr)
			{
				continue;
			}
			item->setLabel(tool->name.c_str());
			item->setFilamentOptions(m_filamentOptions);
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

	void ExtrudePresenter::onActivate()
	{
		ModelLock lock;
		OM::FileSystem::RequestFiles("/filaments",
									 [this]()
									 {
										 ModelLock lock;
										 this->m_filamentOptions.clear();
										 for (size_t i = 0; i < OM::FileSystem::GetItemCount(); i++)
										 {
											 OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(i);
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

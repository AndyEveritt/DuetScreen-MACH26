/*
 * ToolControlPresenter.cpp
 *
 *  Created on: 2025-07-14
 *      Author: Andy Everitt
 */

#include "ToolControlPresenter.h"
#include "Debug.h"
#include "ToolControl.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void ToolControlPresenter::onActivate()
	{
		setToolIndex(2);
		newToolData();

		for (auto& heater : m_view->getHeaters())
		{
			heater->show();
		}
	}

	void ToolControlPresenter::onDeactivate()
	{
		for (auto& heater : m_view->getHeaters())
		{
			auto presenter = heater->getPresenter();
			presenter->deactivate();
		}
	}

	void ToolControlPresenter::reset()
	{
		UI_LOCK();
		m_tool = nullptr;
		m_view->getHeaters().clear();
	}

	void ToolControlPresenter::setToolIndex(size_t toolIndex)
	{
		UI_LOCK();
		reset();
		m_tool = OM::GetTool(toolIndex);
		if (m_tool == nullptr)
		{
			LOG_ERROR("Tool with index {:d} not found", toolIndex);
			return;
		}
		LOG_DBG("Set tool index to {} for presenter '{}'", toolIndex, getName());
	}

	void ToolControlPresenter::toggleToolState()
	{
		if (m_tool == nullptr)
		{
			LOG_ERROR("Tool is null for presenter '{}'", getName());
			return;
		}

		m_tool->ToggleState();
	}

	void ToolControlPresenter::newToolData()
	{
		if (m_tool == nullptr)
		{
			LOG_ERROR("Tool is null for presenter '{}'", getName());
			return;
		}

		LOG_VERBOSE("Updating tool control for tool index {:d}", m_tool->index);
		m_view->setToolName(m_tool->name.c_str());
		m_view->setToolState(m_tool->status, _(m_tool->GetStatusStr()));

		auto& heaters = m_view->getHeaters();

		heaters.setItemCount(m_tool->GetHeaterCount(),
							 [this](size_t index, lv_obj_t* parent)
							 {
								 auto control = std::make_shared<HeaterSlider>(
									 fmt::format("tool_{}_heater_{}", m_tool->index, index), parent);
								 auto presenter = control->getPresenter();
								 presenter->setToolHeaterIndex(m_tool->index, index);
								 presenter->activate();
								 control->setSize(LV_PCT(100), LV_SIZE_CONTENT);
								 return control;
							 });
	}
} // namespace UI

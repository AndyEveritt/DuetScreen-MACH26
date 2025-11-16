/*
 * ToolControlPresenter.cpp
 *
 *  Created on: 2025-07-14
 *      Author: Andy Everitt
 */

#include "ToolControlPresenter.h"
#include "Debug.h"
#include "ToolControl.h"
#include "i18n/i18n.h"

namespace UI
{
	void ToolControlPresenter::onActivate()
	{
		if (m_tool != nullptr)
		{
			setToolIndex(m_toolSlot);
		}
		newToolData();
	}

	void ToolControlPresenter::onDeactivate()
	{
		UI_LOCK();
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
		m_toolSlot = -1;
	}

	void ToolControlPresenter::setToolIndex(size_t toolIndex)
	{
		UI_LOCK();
		reset();
		m_tool = OM::GetToolBySlot(toolIndex);
		if (m_tool == nullptr)
		{
			LOG_ERROR("Tool with index {:d} not found", toolIndex);
			return;
		}
		LOG_DBG("Set tool index to {} for presenter '{}'", toolIndex, getName());
		m_toolSlot = (ssize_t)toolIndex;
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
		m_view->setToolName(m_tool->GetName());
		m_view->setToolState(m_tool->status, _(fmt::format("temperature.status.{:s}", m_tool->GetStatusStr())));

		auto& extrusionFactors = m_view->getExtrusionFactors();
		extrusionFactors.setItemCount(m_tool->GetExtruderCount(),
									  [&](size_t index, LvObj& parent)
									  {
										  auto btn = std::make_unique<Button>(fmt::format("extrusion_factor_{}", index),
																			  parent);
										  btn->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(index)));
										  //   btn->setSize(LV_SIZE_CONTENT, LV_PCT(100));
										  return btn;
									  });
		extrusionFactors.iterateListItems(
			[&](size_t index, Button& btn)
			{
				auto extruder = m_tool->GetExtruder(index);
				if (!extruder)
					return;
				int factor = static_cast<int>(100 * extruder->factor);
				btn.setText(fmt::format("{:d}%", factor));
			});

		auto& heaters = m_view->getHeaters();

		getView()->getFilamentDropdown().setVisible(m_tool->filamentExtruder >= 0);

		heaters.setItemCount(m_tool->GetHeaterCount(),
							 [this](size_t index, LvObj& parent)
							 {
								 auto control = std::make_unique<HeaterSlider>(fmt::format("{}", index), parent);
								 auto presenter = control->getPresenter();
								 presenter->setToolHeaterIndex(m_tool->index, index);
								 control->setNumberPad(m_view->getNumberPad());
								 control->show();
								 return control;
							 });
	}

	void ToolControlPresenter::newToolHeaterData(size_t toolIndex)
	{
		if (m_tool == nullptr)
		{
			LOG_ERROR("Tool is null for presenter '{}'", getName());
			return;
		}
		if (m_tool->index != toolIndex)
		{
			LOG_VERBOSE("Tool index mismatch: expected {:d}, got {:d}", m_tool->index, toolIndex);
			return;
		}

		LOG_VERBOSE("Updating tool heaters for tool index {:d}", m_tool->index);
		m_view->getHeaters().clear();
		newToolData();
	}
} // namespace UI

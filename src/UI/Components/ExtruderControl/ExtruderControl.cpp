/*
 * ExtruderControl.cpp
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#include "ExtruderControl.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	ExtruderControl::ExtruderControl(const std::string& name, lv_obj_t* parent)
		: LvContainer(name, parent)
		, m_toolSelect(name + "_tool_select", getRoot())
		, m_filamentContainer(name + "_filament", getRoot())
		, m_filamentSelect(name + "_filament_select", m_filamentContainer)
		, m_filamentUnloadBtn(name + "_filament_load_unload", m_filamentContainer, _("unload"))
		, m_controlsContainer(name + "_controls", getRoot())
		, m_retractBtn(name + "_retract", m_controlsContainer, LV_SYMBOL_UP)
		, m_extrudeBtn(name + "_extrude", m_controlsContainer, LV_SYMBOL_DOWN)
		, m_distanceInput(name + "_distance_input", m_controlsContainer)
		, m_feedrateInput(name + "_feedrate_input", m_controlsContainer)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

		m_toolSelect.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_filamentContainer.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_controlsContainer.setWidth(LV_PCT(100));
		m_controlsContainer.setFlexGrow(1);

		m_toolSelect.setTitle(_("tool_select"));

		m_filamentContainer.setFlexFlow(LV_FLEX_FLOW_ROW);

		m_filamentSelect.setSize(LV_PCT(80), LV_SIZE_CONTENT);
		m_filamentUnloadBtn.setHeight(LV_PCT(100));
		m_filamentUnloadBtn.setFlexGrow(1);
		m_filamentSelect.setLabel(_("filament_select"));

		static int32_t col_dsc[] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		m_controlsContainer.setGridDsc(col_dsc, row_dsc);
		m_controlsContainer.setGridCell(m_distanceInput, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		m_controlsContainer.setGridCell(m_feedrateInput, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		m_controlsContainer.setGridCell(m_retractBtn, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		m_controlsContainer.setGridCell(m_extrudeBtn, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

		m_distanceInput.setFlag(LV_OBJ_FLAG_CLICKABLE, true);
		m_feedrateInput.setFlag(LV_OBJ_FLAG_CLICKABLE, true);

		m_toolSelect.setListFlow(LV_FLEX_FLOW_ROW);
		m_toolSelect.setListSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_filamentSelect.addEventCallback(onFilamentSelectEvent, LV_EVENT_VALUE_CHANGED, this);
		m_filamentUnloadBtn.addClickedCallback(onFilamentUnloadEvent, this);
		m_retractBtn.addClickedCallback(onRetractEvent, this);
		m_extrudeBtn.addClickedCallback(onExtrudeEvent, this);
		m_distanceInput.addEventCallback(onDistanceEvent, LV_EVENT_CLICKED, this);
		m_feedrateInput.addEventCallback(onFeedrateEvent, LV_EVENT_CLICKED, this);

		m_toolSelect.addStyle(Themes::getLvglStyles().no_border);
		m_filamentContainer.addStyle(Themes::getLvglStyles().no_border);
		m_filamentContainer.addStyle(Themes::getLvglStyles().pad_zero);
		m_filamentSelect.addStyle(Themes::getLvglStyles().no_border);
		m_controlsContainer.addStyle(Themes::getLvglStyles().no_border);
		m_retractBtn.addStyle(Themes::getLvglStyles().actionBtn);
		m_extrudeBtn.addStyle(Themes::getLvglStyles().actionBtn);
		m_filamentUnloadBtn.addStyle(Themes::getLvglStyles().actionBtn);
		m_filamentSelect.getDropdownMenu().addStyle(Themes::getLvglStyles().actionBtn);
		m_distanceInput.addStyle(Themes::getLvglStyles().input);
		m_feedrateInput.addStyle(Themes::getLvglStyles().input);

		setToolCount(3);
		setDistanceValue(10.0f);
		setFeedrateValue(5.0f);
	}

	void ExtruderControl::setToolCallback(tool_select_cb_t cb)
	{
		UI_LOCK();
		LOG_DBG("Setting tool select callback for {}", getName());
		m_toolSelectCb = std::move(cb);
	}

	void ExtruderControl::setToolCount(const size_t count)
	{
		UI_LOCK();
		if (count == m_toolSelect.getItemCount())
		{
			return;
		}

		LOG_DBG("Setting tool count to {} for {}", count, getName());
		m_toolSelect.setItemCount(count, this, &ExtruderControl::createToolButton);
	}

	void ExtruderControl::setCurrentTool(const size_t index)
	{
		UI_LOCK();
		LOG_DBG("Setting current tool to {} for {}", index, getName());
		for (size_t i = 0; i < m_toolSelect.getItemCount(); ++i)
		{
			auto btn = m_toolSelect.getItem(i);
			if (!btn)
			{
				continue;
			}

			btn->setChecked(index == i);
		}
	}

	void ExtruderControl::setFilamentOptions(const std::vector<std::string>& options)
	{
		UI_LOCK();
		LOG_DBG("Setting filament options for {}", getName());
		m_filamentSelect.setOptions(options);
	}

	void ExtruderControl::setFilamentSelected(const std::string& filament)
	{
		UI_LOCK();
		LOG_DBG("Setting selected filament to '{}' for {}", filament, getName());
		m_filamentSelect.setSelected(filament);
	}

	void ExtruderControl::setFilamentCallback(filament_cb_t cb)
	{
		UI_LOCK();
		LOG_DBG("Setting filament callback for {}", getName());
		m_filamentCb = std::move(cb);
	}

	void ExtruderControl::setExtrudeCallback(extrude_cb_t cb)
	{
		UI_LOCK();
		LOG_DBG("Setting extrude callback for {}", getName());
		m_extrudeCb = std::move(cb);
	}

	void ExtruderControl::setDistanceCallback(distance_cb_t cb)
	{
		UI_LOCK();
		LOG_DBG("Setting distance callback for {}", getName());
		m_distanceCb = std::move(cb);
	}

	void ExtruderControl::setFeedrateCallback(feedrate_cb_t cb)
	{
		UI_LOCK();
		LOG_DBG("Setting feedrate callback for {}", getName());
		m_feedrateCb = std::move(cb);
	}

	void ExtruderControl::setDistanceValue(float value)
	{
		UI_LOCK();
		LOG_DBG("Setting distance value to {} for {}", value, getName());
		m_distanceValue = value;
		m_distanceInput.setText(fmt::format(fmt::runtime(_("extrude_distance_input")), value));
	}

	void ExtruderControl::setFeedrateValue(float value)
	{
		UI_LOCK();
		LOG_DBG("Setting feedrate value to {} for {}", value, getName());
		m_feedrateValue = value;
		m_feedrateInput.setText(fmt::format(fmt::runtime(_("extrude_feedrate_input")), value));
	}

	void ExtruderControl::onToolSelectEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto btn = static_cast<lv_obj_t*>(lv_event_get_target(event));
		if (!btn)
		{
			LOG_ERROR("Failed to get button from event");
			return;
		}
		auto control = static_cast<ExtruderControl*>(lv_event_get_user_data(event));

		size_t index = static_cast<size_t>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn)));
		LOG_DBG("Tool button clicked for index {} in {}", index, control->getName());

		if (control && control->m_toolSelectCb)
		{
			LOG_DBG("Calling tool select callback for index {} in {}", index, control->getName());
			control->m_toolSelectCb(index);
		}
		control->setCurrentTool(index);
	}

	void ExtruderControl::onFilamentSelectEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<ExtruderControl*>(lv_event_get_user_data(event));

		if (control && control->m_filamentCb)
		{
			std::string selected = control->m_filamentSelect.getSelectedString();
			LOG_DBG("Calling filament callback for '{}' in {}", selected, control->getName());
			control->m_filamentCb(selected);
		}
	}

	void ExtruderControl::onFilamentUnloadEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<ExtruderControl*>(lv_event_get_user_data(event));

		if (control && control->m_filamentCb)
		{
			LOG_DBG("Calling filament unload callback in {}", control->getName());
			control->m_filamentCb("");
		}
	}

	void ExtruderControl::onDistanceEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<ExtruderControl*>(lv_event_get_user_data(event));
		if (control && control->m_distanceCb)
		{
			LOG_DBG("Calling distance callback with value {} in {}", control->getDistanceValue(), control->getName());
			control->m_distanceCb(control->getDistanceValue());
		}
	}

	void ExtruderControl::onFeedrateEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<ExtruderControl*>(lv_event_get_user_data(event));
		if (control && control->m_feedrateCb)
		{
			LOG_DBG("Calling feedrate callback with value {} in {}", control->getFeedrateValue(), control->getName());
			control->m_feedrateCb(control->getFeedrateValue());
		}
	}

	void ExtruderControl::onRetractEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<ExtruderControl*>(lv_event_get_user_data(event));
		float dist = -control->getDistanceValue();
		float rate = control->getFeedrateValue();

		LOG_DBG("Retracting {} at {} in {}", dist, rate, control->getName());
		if (control && control->m_extrudeCb)
		{
			control->m_extrudeCb(dist, rate);
		}
	}

	void ExtruderControl::onExtrudeEvent(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<ExtruderControl*>(lv_event_get_user_data(event));
		float dist = control->getDistanceValue();
		float rate = control->getFeedrateValue();

		LOG_DBG("Extruding {} at {} in {}", dist, rate, control->getName());
		if (control && control->m_extrudeCb)
		{
			control->m_extrudeCb(dist, rate);
		}
	}

	std::shared_ptr<Button> ExtruderControl::createToolButton(size_t index, lv_obj_t* parent)
	{
		LOG_DBG("Creating tool button {} for {}", index, getName());
		auto btn = std::make_shared<Button>(fmt::format("{}_tool{}", getName(), index), parent);
		btn->setFlexGrow(1);
		btn->setText(fmt::format("tool {}", index));
		btn->setHeight(LV_SIZE_CONTENT);
		btn->addClickedCallback(onToolSelectEvent, this);
		btn->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(index)));
		return btn;
	}
} // namespace UI

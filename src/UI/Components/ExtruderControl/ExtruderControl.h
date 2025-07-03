/*
 * ExtruderControl.h
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/DropdownMenu.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/List/List.h"

namespace UI
{
	class ExtruderControl : public LvContainer
	{
		using tool_select_cb_t = std::function<void(size_t index)>;
		using filament_cb_t = std::function<void(const std::string& filament)>;
		using extrude_cb_t = std::function<void(int32_t distance, uint32_t feedrate)>;
		using distance_cb_t = std::function<void(size_t index, float distance)>;
		using feedrate_cb_t = std::function<void(size_t index, float feedrate)>;

	  public:
		ExtruderControl(const std::string& name, lv_obj_t* parent);

		void setToolCallback(tool_select_cb_t cb);
		void setToolCount(size_t count);
		void setCurrentTool(size_t index);

		void setFilamentOptions(const std::vector<std::string>& options);
		void setFilamentSelected(const std::string& filament);
		void setFilamentCallback(filament_cb_t cb);

		void setExtrudeCallback(extrude_cb_t cb);
		void setDistanceCallback(distance_cb_t cb);
		void setFeedrateCallback(feedrate_cb_t cb);

		void setDistanceValue(size_t index, float value);
		void setFeedrateValue(size_t index, float value);

		float getDistanceValue(size_t index) const;
		float getFeedrateValue(size_t index) const;

	  private:
		static void onToolSelectEvent(lv_event_t* event);
		static void onFilamentSelectEvent(lv_event_t* event);
		static void onFilamentUnloadEvent(lv_event_t* event);
		static void onDistanceEvent(lv_event_t* event);
		static void onFeedrateEvent(lv_event_t* event);
		static void onRetractEvent(lv_event_t* event);
		static void onExtrudeEvent(lv_event_t* event);

		std::shared_ptr<Button> createBaseListButton(const std::string& name, size_t index, lv_obj_t* parent);
		std::shared_ptr<Button> createToolButton(size_t index, lv_obj_t* parent);
		std::shared_ptr<Button> createDistanceButton(size_t index, lv_obj_t* parent);
		std::shared_ptr<Button> createFeedrateButton(size_t index, lv_obj_t* parent);

		List<Button> m_toolSelect;

		LvContainer m_filamentContainer;
		DropdownMenu m_filamentSelect;
		Button m_filamentUnloadBtn;

		LvContainer m_controlsContainer;
		Button m_retractBtn;
		Button m_extrudeBtn;
		List<Button> m_distanceInput;
		List<Button> m_feedrateInput;

		std::vector<float> m_distanceValues;
		std::vector<float> m_feedrateValues;
		size_t m_selectedDistanceIndex;
		size_t m_selectedFeedrateIndex;

		tool_select_cb_t m_toolSelectCb; // Callback for when a tool is selected
		filament_cb_t m_filamentCb;		 // Callback for when a filament selection changes
		extrude_cb_t m_extrudeCb;		 // Callback for when extrude/retract is clicked
		distance_cb_t m_distanceCb;		 // Callback for when distance input is clicked
		feedrate_cb_t m_feedrateCb;		 // Callback for when feedrate input is clicked
	};
} // namespace UI

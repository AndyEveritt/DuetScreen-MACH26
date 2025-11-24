/*
 * ExtruderControl.h
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#pragma once

#include "ExtruderControlPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/DropdownMenu.h"
#include "UI/Components/Input/MultiValueSelector.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/List/List.h"

namespace UI
{
	class ExtruderControl : public View<ExtruderControlPresenter>
	{
		using tool_select_cb_t = std::function<void(size_t index)>;
		using filament_cb_t = std::function<void(const std::string& filament)>;
		using extrude_cb_t = std::function<void(float distance, float feedrate)>;
		using distance_cb_t = std::function<void(size_t index, float distance)>;
		using feedrate_cb_t = std::function<void(size_t index, float feedrate)>;

	  public:
		ExtruderControl(const std::string& name, LvObj& parent);

		void clear();

		void setExtrudeDisabled(bool disabled);
		void setRetractDisabled(bool disabled);
		void setExtrudeCallback(extrude_cb_t cb);

		float getDistanceValue() const;
		float getFeedrateValue() const;

		void setNumberPad(NumberPad* np);
		NumberPad* getNumberPad() const { return m_numberPad; }

	  private:
		static void onRetractEvent(lv_event_t* event);
		static void onExtrudeEvent(lv_event_t* event);

		LvContainer m_controlsContainer{"controls", getRoot()};
		Button m_retractBtn{"retract", m_controlsContainer};
		Button m_extrudeBtn{"extrude", m_controlsContainer};

		MultiValueSelector m_distanceSelector{"distance_selector", m_controlsContainer};
		MultiValueSelector m_feedrateSelector{"feedrate_selector", m_controlsContainer};

		NumberPad* m_numberPad = nullptr; // used for long press callbacks

		extrude_cb_t m_extrudeCb;		 // Callback for when extrude/retract is clicked
		distance_cb_t m_distanceCb;		 // Callback for when distance input is clicked
		feedrate_cb_t m_feedrateCb;		 // Callback for when feedrate input is clicked

		int32_t m_currentToolIndex = -1;
		std::vector<std::string> m_filamentOptions;
		std::string m_loadedFilament;
	};
} // namespace UI

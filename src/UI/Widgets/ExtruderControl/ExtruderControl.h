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
	  public:
		ExtruderControl(const std::string& name, LvObj& parent);

		void clear();

		void setExtrudeDisabled(bool disabled);
		void setRetractDisabled(bool disabled);

		float getDistanceValue() const;
		float getFeedrateValue() const;

		void setNumberPad(ModalNumberPad* np);
		ModalNumberPad* getNumberPad() const { return m_numberPad; }

	  private:
		static void onRetractEvent(lv_event_t* event);
		static void onExtrudeEvent(lv_event_t* event);

		LvContainer m_controlsContainer{"controls", getRoot()};
		Button m_retractBtn{"retract", m_controlsContainer};
		Button m_extrudeBtn{"extrude", m_controlsContainer};

		MultiValueSelector m_distanceSelector{"distance_selector", m_controlsContainer};
		MultiValueSelector m_feedrateSelector{"feedrate_selector", m_controlsContainer};

		ModalNumberPad* m_numberPad = nullptr; // used for long press callbacks
	};
} // namespace UI

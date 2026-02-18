/*
 * ToolControl.h
 *
 *  Created on: 2025-07-14
 *      Author: Andy Everitt
 */

#pragma once

#include "HeaterSlider.h"
#include "ToolControlPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/List/List.h"
#include "UI/Core/View.h"
#include "UI/Widgets/Control/ExtrusionFactor/ExtrusionFactor.h"
#include "UI/Widgets/Filament/Dropdown/FilamentSelectDropdown.h"

namespace UI
{
	class ToolControl : public View<ToolControlPresenter>
	{
	  public:
		ToolControl(const std::string& name, LvObj& parent);

		void setToolName(std::string_view name);
		void setToolState(ToolControlPresenter::tool_state_t state, std::string_view str);
		List<Button>& getExtrusionFactors() { return m_extrusionFactors; }
		FilamentSelectDropdown& getFilamentDropdown() { return m_filament; }
		List<HeaterSlider>& getHeaters() { return m_heaters; }

		void setDisabled(bool disabled);

		void setNumberPad(ModalNumberPad* numberPad);
		auto getNumberPad() { return m_numberPad; }

		void setExtrusionModal(ModalExtrusionFactor* modal);
		auto getExtrusionModal() { return m_extrusionFactorModal; }

	  private:
		LvContainer m_toolInfoCont{"tool_info", getRoot()};
		Button m_toolName{"tool_name", m_toolInfoCont};
		LvLabel m_state{"tool_state", m_toolInfoCont};
		LvContainer m_toolInfoSpacer{"tool_info_spacer", m_toolInfoCont};
		List<Button> m_extrusionFactors{"extrusion_factors", m_toolInfoCont};
		FilamentSelectDropdown m_filament{"filament_select", m_toolInfoCont};
		List<HeaterSlider> m_heaters{"heaters", getRoot()};

		ModalNumberPad* m_numberPad = nullptr;
		ModalExtrusionFactor* m_extrusionFactorModal = nullptr;
	};
} // namespace UI

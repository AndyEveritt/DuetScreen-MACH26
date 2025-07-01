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

namespace UI
{
	class ExtruderControl : public LvContainer
	{
	  public:
		ExtruderControl(const std::string& name, lv_obj_t* parent);

	  private:
		DropdownMenu m_toolSelect;

		LvContainer m_filamentContainer;
		DropdownMenu m_filamentSelect;
		Button m_filamentLoadUnloadBtn;

		LvContainer m_controlsContainer;
		Button m_retractBtn;
		Button m_extrudeBtn;
		TextBox m_distanceInput;
		TextBox m_feedrateInput;
	};
} // namespace UI

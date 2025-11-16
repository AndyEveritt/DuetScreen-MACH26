/*
 * MultiValueSelector.h
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/List/List.h"

namespace UI
{
	class MultiValueSelector : public LvContainer
	{
	  public:
		MultiValueSelector(const std::string& name, LvObj& parent);

        void setValues(const std::vector<float>& values);

	  private:
        LvContainer m_topRow{"topRow", getRoot()};
        Button m_decrement{"decrement", m_topRow};
        TextBox m_valueDisplay{"valueDisplay", m_topRow};
        Button m_increment{"increment", m_topRow};
        List<Button> m_values{"values", getRoot()};

        std::vector<float> m_currentValues;
	};
} // namespace UI

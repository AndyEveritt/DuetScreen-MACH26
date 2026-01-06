/*
 * LvKeyboard.h
 *
 *  Created on: 2025-06-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvTextarea.h"
#include "UI/Components/LVGL/generated/LvKeyboard.gen.h"

namespace UI
{

	class LvKeyboard : public LvKeyboardGen
	{
	  public:
		LvKeyboard(const std::string& name, LvObj& parent);

		void setTextarea(LvTextarea* ta);
		LvTextarea* getTextarea() const;

	  private:
	};
} // namespace UI

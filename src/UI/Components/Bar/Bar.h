/*
 * Bar.h
 *
 *  Created on: 2026-01-09
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvBar.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/LVGL/LvSubject.h"

namespace UI
{
	class Bar : public LvBar
	{
	  public:
		Bar(const std::string& name, LvObj& parent);

		void setLabelFormat(const std::string& fmt);

	  private:
		static void barEvent(lv_event_t* e);
		static void drawLabel(lv_event_t* e);

		std::string m_labelFmt{"{}"};
	};
} // namespace UI

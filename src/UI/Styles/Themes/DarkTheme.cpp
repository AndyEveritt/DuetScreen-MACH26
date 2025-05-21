/*
 * DarkTheme.cpp
 *
 *  Created on: 2025-05-21
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	class DarkTheme : public Theme
	{
	  public:
		DarkTheme(const char* name)
			: Theme(name)
		{
		}

		virtual void init() override
		{
			lv_style_set_bg_color(m_container, lv_color_hex(0x2E3440));
			lv_style_set_bg_opa(m_container, LV_OPA_COVER);
			lv_style_set_border_color(m_container, lv_palette_main(LV_PALETTE_TEAL));

			lv_style_set_border_color(m_button, lv_palette_main(LV_PALETTE_RED));
		}
	};

	static DarkTheme s_darkTheme("dark");
} // namespace UI::Themes

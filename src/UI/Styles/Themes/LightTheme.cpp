/*
 * LightTheme.cpp
 *
 *  Created on: 2025-05-21
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	class LightTheme : public Theme
	{
	  public:
		LightTheme(const char* name)
			: Theme(name)
		{
		}

		virtual void init() override
		{
			lv_style_set_bg_color(m_base, lv_color_hex(0xDE3440));
			lv_style_set_bg_opa(m_base, LV_OPA_COVER);
			lv_style_set_border_color(m_base, lv_palette_main(LV_PALETTE_BLUE));

			lv_style_set_bg_color(m_button, lv_palette_main(LV_PALETTE_GREEN));
		}
	};

	static LightTheme s_lightTheme("light");
} // namespace UI::Themes

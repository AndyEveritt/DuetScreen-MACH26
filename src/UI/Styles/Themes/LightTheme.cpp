/*
 * LightTheme.cpp
 *
 *  Created on: 2025-05-21
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Styles/Styles.h"

#define COLOR_SCR lv_palette_lighten(LV_PALETTE_GREY, 4)
#define COLOR_CARD lv_color_white()
#define COLOR_TEXT lv_palette_darken(LV_PALETTE_GREY, 4)
#define COLOR_BORDER lv_palette_lighten(LV_PALETTE_GREY, 2)

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
			lv_style_set_bg_color(m_lvglStyles.screen, COLOR_SCR);
			lv_style_set_text_color(m_lvglStyles.screen, COLOR_TEXT);
			lv_style_set_text_color(m_lvglStyles.card, COLOR_TEXT);
			lv_style_set_bg_color(m_lvglStyles.card, COLOR_CARD);
			lv_style_set_border_color(m_lvglStyles.card, COLOR_BORDER);

			lv_style_set_bg_color(m_lvglStyles.btn, lv_palette_main(LV_PALETTE_GREEN));

			lv_style_set_bg_color(m_estop, lv_palette_main(LV_PALETTE_RED));
			// lv_style_set_text_color(m_estop, COLOR_TEXT);
		}
	};

	static LightTheme s_lightTheme("theme_light");
} // namespace UI::Themes

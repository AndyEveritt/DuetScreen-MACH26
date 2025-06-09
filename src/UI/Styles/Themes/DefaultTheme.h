/*
 * DefaultTheme.h
 *
 *  Created on: 2025-06-02
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Styles/Styles.h"

namespace UI::Themes
{

	class DefaultTheme : public Theme
	{
	  public:
		DefaultTheme(const char* name,
					 lv_color_t primaryColor,
					 lv_color_t secondaryColor,
					 lv_color_t cardColor,
					 lv_color_t textColor,
					 lv_color_t highlightColor,
					 const lv_font_t* font,
					 bool darkMode,
					 std::function<void(Theme* theme)> styleOverrides = nullptr)
			: Theme(name, styleOverrides)
			, m_primaryColor(primaryColor)
			, m_secondaryColor(secondaryColor)
			, m_cardColor(cardColor)
			, m_textColor(textColor)
			, m_highlightColor(highlightColor)
			, m_fontSmall(font)
			, m_fontNormal(font)
			, m_fontLarge(font)
			, m_darkMode(darkMode)
		{
		}

	  protected:
		lv_color_t m_primaryColor;
		lv_color_t m_secondaryColor;
		lv_color_t m_cardColor;
		lv_color_t m_textColor;
		lv_color_t m_highlightColor;
		const lv_font_t* m_fontSmall;
		const lv_font_t* m_fontNormal;
		const lv_font_t* m_fontLarge;
		bool m_darkMode;

		lv_style_transition_dsc_t m_draggableTransition;
		lv_style_transition_dsc_t m_draggingTransition;
		lv_style_transition_dsc_t m_dragCompleteTransition;

	  private:
		virtual void onInit() override;
	};
} // namespace UI::Themes

/*
 * ThemePreview.h
 *
 *  Created on: 2025-08-06
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Canvas/Canvas.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/Input/Slider.h"
#include "UI/Components/LVGL/LvCheckbox.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvSwitch.h"
#include "UI/Components/List/List.h"

namespace UI
{
	class Swatch;

	class ThemePreview : public LvContainer
	{
	  public:
		using ToggleWidget = UI_SETTINGS_TOGGLE_WIDGET;

		ThemePreview(const std::string& name, LvObj& parent);
		virtual ~ThemePreview(); // = default but Swatch is forward declared;

		void updateSwatches();
		void updateSliders(size_t primaryHue, size_t secondaryHue, float chroma, bool darkMode);
		void setPrimaryHue(size_t hue);
		void setSecondaryHue(size_t hue);
		void setChroma(float chroma);
		void setDarkMode(bool enable);

		void showControls(bool show);

		void setNumberPad(ModalNumberPad* numberPad);

	  private:
		void onShow() override;

		void updateThemeColors();
		void renderColorPreview(Canvas& canvas, float luminance, float chroma);

		LvLabel m_primaryHueLabel{"primary_hue_label", getRoot()};
		LvLabel m_secondaryHueLabel{"secondary_hue_label", getRoot()};
		LvLabel m_chromaLabel{"chroma_label", getRoot()};
		LvLabel m_darkModeLabel{"dark_mode_label", getRoot()};

		List<Swatch> m_swatches;
		Canvas m_primaryColorPreview{"primary_color_preview", getRoot()};
		Slider m_primaryHueSlider{"primary_hue_slider", getRoot()};
		Canvas m_secondaryColorPreview{"secondary_color_preview", getRoot()};
		Slider m_secondaryHueSlider{"secondary_hue_slider", getRoot()};
		Slider m_chromaSlider{"chroma_slider", getRoot()};
		ToggleWidget m_darkMode{"dark_mode", getRoot()};

		ModalNumberPad* m_numberPad = nullptr;
	};
} // namespace UI

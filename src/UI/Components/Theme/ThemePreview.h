/*
 * ThemePreview.h
 *
 *  Created on: 2025-08-06
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Input/Slider.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/List/List.h"

namespace UI
{
	class Swatch;

	class ThemePreview : public LvContainer
	{
	  public:
		ThemePreview(const std::string& name, lv_obj_t* parent);

		void updateSwatches();

	  private:
		void updateThemeColors();

		List<Swatch> m_swatches;
		Slider m_primaryHueSlider;
		Slider m_secondaryHueSlider;
		Slider m_chromaSlider;
	};
} // namespace UI

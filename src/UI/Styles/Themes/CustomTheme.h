/*
 * CustomTheme.h
 *
 *  Created on: 2025-10-24
 *      Author: Andy Everitt
 */

#pragma once
#include "DefaultTheme.h"
#include "Storage.h"

namespace UI::Themes
{
	class CustomTheme : public DefaultTheme
	{
	  public:
		CustomTheme(std::string_view name,
					FontConfigSet fontConfigSet,
					std::function<void(Theme* theme)> styleOverrides = nullptr);

		void setColors(uint16_t primaryHue, uint16_t secondaryHue, float chroma, bool darkMode);

		uint16_t getPrimaryHue() const { return m_primaryHue; }
		uint16_t getSecondaryHue() const { return m_secondaryHue; }
		float getChroma() const { return m_chroma; }
		bool getDarkMode() const { return m_darkMode; }

	  protected:
		void onInit() override;

	  private:
		uint16_t m_primaryHue;
		uint16_t m_secondaryHue;
		float m_chroma;
		bool m_darkMode;

		bool m_initialized = false;

		struct
		{
			StorageKeyRunTime<uint16_t> primaryHue;
			StorageKeyRunTime<uint16_t> secondaryHue;
			StorageKeyRunTime<float> chroma;
			StorageKeyRunTime<bool> darkMode;
		} m_storageKeys;
	};
} // namespace UI::Themes

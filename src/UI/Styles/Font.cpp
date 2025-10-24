/*
 * Font.cpp
 *
 *  Created on: 2025-10-23
 *      Author: Andy Everitt
 */

#include "Font.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include <filesystem>

#define FONTS_FOLDER ASSETS_FOLDER "fonts/"

namespace UI::Font
{
	static lv_font_manager_t* s_fontManager = nullptr;
	static std::vector<std::string> s_loadedFontNames;

	/**
	 * @brief Load all fonts from the fonts directory
	 */
	void init()
	{
		s_fontManager = lv_font_manager_create(8);

		std::filesystem::directory_iterator dir_iter(FONTS_FOLDER);
		for (const auto& entry : dir_iter)
		{
			if (!entry.is_regular_file())
			{
				continue;
			}
			const auto& path = entry.path();
			const std::string name = path.stem().string();

			bool success = false;
			if (path.extension() == ".bin")
			{
				LOG_DBG("Loading .bin font: {:s}", path.filename().c_str());
				success = lv_font_manager_add_src(s_fontManager, name.c_str(), path.c_str(), &lv_binfont_font_class);
			}
			else if (path.extension() == ".ttf")
			{
				LOG_DBG("Loading FreeType font: {:s}", path.filename().c_str());
				success = lv_font_manager_add_src(s_fontManager, name.c_str(), path.c_str(), &lv_freetype_font_class);
			}

			if (!success)
			{
				LOG_ERROR("Failed to load font: {:s}", path.c_str());
				continue;
			}

			LOG_VERBOSE("Loaded font: {:s}", name);
			s_loadedFontNames.push_back(name);
		}
	}

	lv_font_t* createFont(const std::string& name, uint32_t size)
	{
		if (!s_fontManager)
		{
			LOG_ERROR("Font manager not initialized");
			return nullptr;
		}

		lv_font_t* font = lv_font_manager_create_font(s_fontManager,
													  name.c_str(),
													  LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
													  size,
													  LV_FREETYPE_FONT_STYLE_NORMAL,
													  LV_FONT_KERNING_NONE);
		if (!font)
		{
			LOG_ERROR("Failed to create font '{:s}' with size {}", name, size);
		}
		return font;
	}
} // namespace UI::Font

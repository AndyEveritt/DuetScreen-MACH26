/*
 * i18n.cpp
 *
 *  Created on: 2025-10-13
 *      Author: Andy Everitt
 */

#include "i18n.h"
#include "Configuration.h"
#include "Debug.h"
#include "utils/StorageHelper.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

namespace i18n
{
#if SIMULATION
#  define LANG_DIR "assets/i18n"
#else
#  define LANG_DIR "/etc/assets/i18n"
#endif
#define LANG_FILE_EXT ".json"
	using language_code_t = std::string;
	using language_readable_t = std::string;
	using language_table_t = std::map<language_code_t, language_readable_t>;
	static language_code_t s_currentLanguage;
	static language_table_t s_languages;

	struct string_hash
	{
		using is_transparent = void; // Enable heterogeneous lookup

		size_t operator()(const std::string& str) const noexcept { return std::hash<std::string_view>{}(str); }
		size_t operator()(std::string_view str) const noexcept { return std::hash<std::string_view>{}(str); }
		size_t operator()(const char* str) const noexcept { return std::hash<std::string_view>{}(str); }
	};

	using translation_key_t = std::string;
	using translation_value_t = std::string;
	using translation_table_t =
		std::unordered_map<translation_key_t, translation_value_t, string_hash, std::equal_to<>>;
	static translation_table_t s_translationTable;

	static bool loadLanguageFile(const std::filesystem::path& filepath);
	static std::string parseLanguageReadableName(const std::filesystem::path& filepath);

	void init()
	{
		LOG_INFO("Initialising i18n module...");
		refreshLanguageFiles();

		setLanguage(StorageHelper::getData<std::string>(ID_SYS_LANG_CODE_KEY, DEFAULT_LANGUAGE_CODE));
	}

	std::string_view getCurrentLanguage()
	{
		return s_currentLanguage;
	}

	void refreshLanguageFiles()
	{
		LOG_DBG("Refreshing language files...");
		s_languages.clear();
		std::ranges::for_each(std::filesystem::directory_iterator(LANG_DIR),
							  [](const auto& entry)
							  {
								  if (entry.is_regular_file() && entry.path().extension() == LANG_FILE_EXT)
								  {
									  std::string lang_code = entry.path().stem().string();
									  std::string readable = parseLanguageReadableName(entry.path());

									  s_languages[lang_code] = readable;
								  }
							  });
	}

	const std::map<language_code_t, language_readable_t>& getAvailableLanguages()
	{
		return s_languages;
	}

	bool setLanguage(const std::string_view lang)
	{
		LOG_INFO("Setting language to {:s}", lang);
		std::filesystem::path lang_file = fmt::format(LANG_DIR "/{:s}" LANG_FILE_EXT, lang);
		if (!std::filesystem::exists(lang_file))
		{
			LOG_ERROR("Language file not found: {:s}", lang);
			return false;
		}

		if (!loadLanguageFile(lang_file))
		{
			return false;
		}

		s_currentLanguage = lang;
		return true;
	}

	const std::string& translate(std::string_view tag)
	{
		/* Find the translation if it exists */
		{
			auto it = s_translationTable.find(tag);
			if (it != s_translationTable.end())
			{
				return it->second;
			}
		}

		/* Add the missing translation key to the table */
		{
			LOG_WARN("Missing translation for key '{:s}'", tag);
			auto [it, inserted] = s_translationTable.emplace(std::string(tag), std::string(tag));
			if (!inserted)
			{
				LOG_FATAL_THROW("Failed to insert missing translation key '{:s}'", tag);
			}
			return it->second;
		}
	}

	static bool loadLanguageFile(const std::filesystem::path& filepath)
	{
		LOG_DBG("Loading language file: {:s}...", filepath.string());
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open language file: {:s}", filepath.string());
			return false;
		}
		nlohmann::json j;
		try
		{
			/* Parse json and ignore comments */
			j = nlohmann::json::parse(file, nullptr, true, true);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Error reading JSON file: {}", e.what());
			return false;
		}
		auto data = j.get<std::map<std::string, nlohmann::json>>();

		if (data.find("readable") == data.end() || !data["readable"].is_string())
		{
			LOG_ERROR("Language file missing 'readable' field or it is not a string: {:s}", filepath.string());
			return false;
		}
		std::string readable = data["readable"].get<std::string>();

		if (data.find("translations") == data.end() || !data["translations"].is_object())
		{
			LOG_ERROR("Language file missing 'translations' field or it is not an object: {:s}", filepath.string());
			return false;
		}
		auto translations = data["translations"].get<std::map<std::string, nlohmann::json>>();

		// Extract language code from filename
		std::string lang_code = filepath.filename().stem().string();

		// Add to languages
		s_languages[lang_code] = readable;

		// Add to translation table
		std::string full_key;
		// Flatten arbitrarily nested translation objects into dot-separated keys
		s_translationTable.clear();

		auto flatten = [&](const auto& self, const nlohmann::json& node, const std::string& prefix) -> void
		{
			if (node.is_object())
			{
				for (const auto& [k, v] : node.items())
				{
					std::string next = prefix.empty() ? k : (prefix + "." + k);
					self(self, v, next);
				}
			}
			else if (node.is_string())
			{
				s_translationTable[prefix] = node.get<std::string>();
			}
			else
			{
				LOG_WARN("Translation value for key '{}' is not a string, skipping", prefix);
			}
		};

		flatten(flatten, data["translations"], "");

		return true;
	}

	static std::string parseLanguageReadableName(const std::filesystem::path& filepath)
	{
		LOG_DBG("Parsing language readable name from file: {:s}...", filepath.string());
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open language file: {:s}", filepath.string());
			return "";
		}

		nlohmann::json j;
		try
		{
			/* Parse json and ignore comments */
			j = nlohmann::json::parse(file,
									  /* callback */ nullptr,
									  /* allow_exceptions */ true,
									  /* ignore_comments */ true);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Error reading JSON file: {}", e.what());
			return "";
		}

		if (j.find("readable") != j.end() && j["readable"].is_string())
		{
			return j["readable"].get<std::string>();
		}
		return "";
	}
} // namespace i18n

#pragma once

#include "Debug.h"
#include "Storage.h"
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

class StorageHelper
{
  public:
	template <typename T>
	static void setData(std::string_view key, const T& value)
	{
		ZoneScoped;
		LOG_VERBOSE("Saving \"{:s}\" to config.json", key);
		nlohmann::json* current = &data_;

		std::size_t pos = 0;
		while (pos <= key.size())
		{
			const std::size_t next = key.find(':', pos);
			const std::string_view segment =
				(next == std::string_view::npos) ? key.substr(pos) : key.substr(pos, next - pos);
			current = &(*current)[segment];
			if (next == std::string_view::npos)
			{
				break;
			}
			pos = next + 1;
		}
		if (*current == value)
		{
			return;
		}
		*current = value;
		save();
	}

	static bool setConfigFile(std::string_view filename);
	static bool save();
	static bool load();
	static bool clear();

	template <typename T>
	static T getData(std::string_view key, const T& defaultValue)
	{
		ZoneScoped;
		nlohmann::json* current = &data_;

		std::size_t pos = 0;
		while (pos <= key.size())
		{
			const std::size_t next = key.find(':', pos);
			const std::string_view segment =
				(next == std::string_view::npos) ? key.substr(pos) : key.substr(pos, next - pos);
			// const std::string segmentStr(segment);
			if (!current->contains(segment))
			{
				return defaultValue;
			}
			current = &(*current)[segment];
			if (next == std::string_view::npos)
			{
				break;
			}
			pos = next + 1;
		}

		return current->get<T>();
	}

  private:
	inline static nlohmann::json data_;
};

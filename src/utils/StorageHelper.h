#pragma once

#include "Debug.h"
#include "Storage.h"
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

class StorageHelper
{
  public:
	template <typename T>
	static void setData(const std::string& key, const T& value)
	{
		LOG_VERBOSE("Saving \"{:s}\" to config.json", key.c_str());
		std::istringstream keyStream(key);
		std::string segment;
		nlohmann::json* current = &data_;

		while (std::getline(keyStream, segment, ':'))
		{
			current = &(*current)[segment];
		}
		*current = value;
		save();
	}

	static bool save();
	static bool load();
	static bool clear();

	template <typename T>
	static T getData(const std::string& key, const T& defaultValue)
	{
		std::istringstream keyStream(key);
		std::string segment;
		nlohmann::json* current = &data_;

		while (std::getline(keyStream, segment, ':'))
		{
			if (current->find(segment) == current->end())
			{
				return defaultValue;
			}
			current = &(*current)[segment];
		}

		return current->get<T>();
	}

  private:
	static nlohmann::json data_;
};

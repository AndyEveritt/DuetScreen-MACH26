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
		verbose("Saving \"%s\" to config.json", key.c_str());
		data_[key] = value;
		save();
	}

	static bool save();
	static bool load();
	static bool clear();

	template <typename T>
	static T getData(const std::string& key, const T& defaultValue)
	{
		if (data_.find(key) == data_.end())
		{
			return defaultValue;
		}
		return data_.at(key).get<T>();
	}

  private:
	static nlohmann::json data_;
};

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

class StorageHelper
{
  public:
	template <typename T>
	static void addData(const std::string& key, const T& value)
	{
		data_[key] = value;
		save();
	}

	static bool save();
	static bool load();
	static bool clear();

	template <typename T>
	static T getData(const std::string& key)
	{
		return data_.at(key).get<T>();
	}

  private:
	static std::map<std::string, nlohmann::json> data_;
};

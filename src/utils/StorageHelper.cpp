#include "StorageHelper.h"

static std::string filename_ = "config.json";

nlohmann::json StorageHelper::data_ = {};

bool StorageHelper::save()
{
	std::ofstream file(filename_);
	if (!file.is_open())
	{
		return false;
	}
	nlohmann::json j(data_);
	file << j.dump();
	file.close();
	return true;
}

bool StorageHelper::load()
{
	std::ifstream file(filename_);
	if (!file.is_open())
	{
		return false;
	}
	nlohmann::json j;
	file >> j;
	data_ = j.get<std::map<std::string, nlohmann::json>>();
	file.close();
	return true;
}

bool StorageHelper::clear()
{
	data_.clear();
	return save();
}

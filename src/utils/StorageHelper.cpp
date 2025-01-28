#include "StorageHelper.h"

#if T113
static std::string filename_ = "/etc/duetscreen.json";
#elif SIMULATION
static std::string filename_ = "config.json";
#endif

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

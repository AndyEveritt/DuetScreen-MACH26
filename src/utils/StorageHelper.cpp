#include "StorageHelper.h"

#if T113
static std::string filename_ = "/etc/duetscreen.json";
#elif SIMULATION
static std::string filename_ = "config.json";
#endif

bool StorageHelper::setConfigFile(std::string_view filename)
{
	filename_ = filename;
	if (!load())
	{
		LOG_ERROR("Failed to load config file: {:s}", filename_.c_str());
		return false;
	}

	LOG_INFO("Loaded config file: {:s}", filename_.c_str());
	return true;
}

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
	try
	{
		file >> j;
	}
	catch (const std::exception& e)
	{
		printf("Error reading JSON file\n");
		file.close();
		std::ofstream recreate_file(filename_);
		if (!recreate_file.is_open())
		{
			return false;
		}
		recreate_file << "{}";
		recreate_file.close();
		j = nlohmann::json::object();
	}
	data_ = j.get<std::map<std::string, nlohmann::json>>();
	if (file.is_open())
	{
		file.close();
	}
	return true;
}

bool StorageHelper::clear()
{
	data_.clear();
	return save();
}

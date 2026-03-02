#include "StorageHelper.h"
#include <cstdio>
#include <unistd.h>

#if T113
static std::string filename_ = "/etc/duetscreen.json";
#elif SIMULATION
static std::string filename_ = "config.json";
#endif

bool StorageHelper::setConfigFile(std::string_view filename)
{
	ZoneScoped;
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
	ZoneScoped;
	nlohmann::json j(data_);
	auto json_string = j.dump(2);
	LOG_DBG("Saving config.json:\n{:s}", json_string);

	FILE* file = fopen(filename_.c_str(), "w");
	if (!file)
	{
		return false;
	}

	if (fwrite(json_string.c_str(), 1, json_string.length(), file) != json_string.length())
	{
		fclose(file);
		return false;
	}

	fflush(file);
	fsync(fileno(file));
	fclose(file);
	return true;
}

bool StorageHelper::load()
{
	ZoneScoped;
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
	ZoneScoped;
	data_.clear();
	return save();
}

std::string StorageHelper::dump()
{
	ZoneScoped;
	nlohmann::json j(data_);
	return j.dump(2);
}

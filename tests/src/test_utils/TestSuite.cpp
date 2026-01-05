/*
 * TestSuite.cpp
 *
 *  Created on: 2025-10-07
 *      Author: Andy Everitt
 */

#include "TestSuite.h"
#include "Comm/JsonDecoder.h"
#include "Configuration.h"
#include "DeadlockDetector.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Utils.h"
#include "test_utils/utils.h"
#include "utils/StorageHelper.h"
#include <filesystem>
#include <fstream>

TestSuite::TestSuite()
{
	/* Run at start of each test */
	std::filesystem::remove("tests/config.json");
	StorageHelper::setConfigFile("tests/config.json");
	Log::SetDebugLevel(Log::DebugLevel::Debug);
	Log::Init();

	Comm::init();

	DeadlockDetector::getInstance().allowThreadToTakeMultipleLocks(Log::GetThreadId(), true);

	OM::RemoveAll();

	std::filesystem::remove_all("/tmp/thumbnails");
	std::filesystem::create_directories("/tmp/thumbnails");
}

TestSuite::~TestSuite()
{
	/* Run at end of each test */
	OM::RemoveAll();
	std::filesystem::remove_all("/tmp/thumbnails");
}

bool TestSuite::load_model_data_from_file(std::string_view filename)
{
	// Read from file and send data to JsonDecoder

	std::ifstream file(filename.data());

	EXPECT_TRUE(file.is_open());

	std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return load_model_data(data);
}

bool TestSuite::load_model_data(std::string_view data)
{
	Comm::JsonDecoder decoder;
	decoder.CheckInput(reinterpret_cast<const unsigned char*>(data.data()), static_cast<unsigned int>(data.size()));
	return true;
}
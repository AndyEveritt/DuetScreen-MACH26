/*
 * Directories.h
 *
 *  Created on: 2025-04-09
 *      Author: Andy Everitt
 */

#pragma once

#include <string>

namespace OM::Directories
{
	enum class DirectoryType
	{
		FILAMENTS,
		FIRMWARE,
		GCODES,
		MACROS,
		MENU,
		SYSTEM,
		WEB
	};

	const std::string& GetDirectory(DirectoryType type);

	const std::string& GetFilamentsDirectory();
	const std::string& GetFirmwareDirectory();
	const std::string& GetGcodesDirectory();
	const std::string& GetMacrosDirectory();
	const std::string& GetMenuDirectory();
	const std::string& GetSystemDirectory();
	const std::string& GetWebDirectory();

	void SetFilamentsDirectory(const std::string& dir);
	void SetFirmwareDirectory(const std::string& dir);
	void SetGcodesDirectory(const std::string& dir);
	void SetMacrosDirectory(const std::string& dir);
	void SetMenuDirectory(const std::string& dir);
	void SetSystemDirectory(const std::string& dir);
	void SetWebDirectory(const std::string& dir);

	void Reset();
} // namespace OM::Directories
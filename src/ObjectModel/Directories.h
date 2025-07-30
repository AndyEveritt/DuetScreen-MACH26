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

	std::string_view GetDirectory(DirectoryType type);

	std::string_view GetFilamentsDirectory();
	std::string_view GetFirmwareDirectory();
	std::string_view GetGcodesDirectory();
	std::string_view GetMacrosDirectory();
	std::string_view GetMenuDirectory();
	std::string_view GetSystemDirectory();
	std::string_view GetWebDirectory();

	void SetFilamentsDirectory(std::string_view dir);
	void SetFirmwareDirectory(std::string_view dir);
	void SetGcodesDirectory(std::string_view dir);
	void SetMacrosDirectory(std::string_view dir);
	void SetMenuDirectory(std::string_view dir);
	void SetSystemDirectory(std::string_view dir);
	void SetWebDirectory(std::string_view dir);

	void Reset();
} // namespace OM::Directories
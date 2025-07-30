/*
 * Directories.cpp
 *
 *  Created on: 2025-04-09
 *      Author: Andy Everitt
 */

#include "Directories.h"
#include "Debug.h"

namespace OM::Directories
{
	static std::string s_filaments;
	static std::string s_firmware;
	static std::string s_gcodes;
	static std::string s_macros;
	static std::string s_menu;
	static std::string s_system;
	static std::string s_web;

	static std::string s_emptyStr = "";

	std::string_view GetDirectory(DirectoryType type)
	{
		switch (type)
		{
		case DirectoryType::FILAMENTS:
			return GetFilamentsDirectory();
		case DirectoryType::FIRMWARE:
			return GetFirmwareDirectory();
		case DirectoryType::GCODES:
			return GetGcodesDirectory();
		case DirectoryType::MACROS:
			return GetMacrosDirectory();
		case DirectoryType::MENU:
			return GetMenuDirectory();
		case DirectoryType::SYSTEM:
			return GetSystemDirectory();
		case DirectoryType::WEB:
			return GetWebDirectory();
		default:
			LOG_ERROR("Invalid directory type");
			return s_emptyStr;
		}
	}

	std::string_view GetFilamentsDirectory()
	{
		return s_filaments;
	}
	std::string_view GetFirmwareDirectory()
	{
		return s_firmware;
	}
	std::string_view GetGcodesDirectory()
	{
		return s_gcodes;
	}
	std::string_view GetMacrosDirectory()
	{
		return s_macros;
	}
	std::string_view GetMenuDirectory()
	{
		return s_menu;
	}
	std::string_view GetSystemDirectory()
	{
		return s_system;
	}
	std::string_view GetWebDirectory()
	{
		return s_web;
	}

	void SetFilamentsDirectory(std::string_view dir)
	{
		s_filaments = dir;
	}
	void SetFirmwareDirectory(std::string_view dir)
	{
		s_firmware = std::string(dir);
	}
	void SetGcodesDirectory(std::string_view dir)
	{
		s_gcodes = std::string(dir);
	}
	void SetMacrosDirectory(std::string_view dir)
	{
		s_macros = std::string(dir);
	}
	void SetMenuDirectory(std::string_view dir)
	{
		s_menu = std::string(dir);
	}
	void SetSystemDirectory(std::string_view dir)
	{
		s_system = std::string(dir);
	}
	void SetWebDirectory(std::string_view dir)
	{
		s_web = std::string(dir);
	}

	void Reset()
	{
		s_filaments = "0:/filaments/";
		s_firmware = "0:/firmware/";
		s_gcodes = "0:/gcodes/";
		s_macros = "0:/macros/";
		s_menu = "0:/menu/";
		s_system = "0:/sys/";
		s_web = "0:/www/";
	}
} // namespace OM::Directories
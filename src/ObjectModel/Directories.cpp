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

	const std::string& GetFilamentsDirectory()
	{
		return s_filaments;
	}
	const std::string& GetFirmwareDirectory()
	{
		return s_firmware;
	}
	const std::string& GetGcodesDirectory()
	{
		return s_gcodes;
	}
	const std::string& GetMacrosDirectory()
	{
		return s_macros;
	}
	const std::string& GetMenuDirectory()
	{
		return s_menu;
	}
	const std::string& GetSystemDirectory()
	{
		return s_system;
	}
	const std::string& GetWebDirectory()
	{
		return s_web;
	}

	void SetFilamentsDirectory(const std::string& dir)
	{
		s_filaments = dir;
	}
	void SetFirmwareDirectory(const std::string& dir)
	{
		s_firmware = dir;
	}
	void SetGcodesDirectory(const std::string& dir)
	{
		s_gcodes = dir;
	}
	void SetMacrosDirectory(const std::string& dir)
	{
		s_macros = dir;
	}
	void SetMenuDirectory(const std::string& dir)
	{
		s_menu = dir;
	}
	void SetSystemDirectory(const std::string& dir)
	{
		s_system = dir;
	}
	void SetWebDirectory(const std::string& dir)
	{
		s_web = dir;
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
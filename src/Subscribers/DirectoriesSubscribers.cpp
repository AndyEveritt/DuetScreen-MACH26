/*
 * DirectoriesSubscribers.cpp
 *
 *  Created on: 2025-04-09
 *      Author: Andy Everitt
 */

#include "DirectoriesSubscribers.h"
#include "Debug.h"

#include "ObjectModel/Directories.h"
#include "UI/Core/Model.h"

bool DirectoriesSubscribers::filaments(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Directories::SetFilamentsDirectory(data);
	return true;
}

bool DirectoriesSubscribers::firmware(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Directories::SetFirmwareDirectory(data);
	return true;
}

bool DirectoriesSubscribers::gcodes(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Directories::SetGcodesDirectory(data);
	return true;
}

bool DirectoriesSubscribers::macros(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Directories::SetMacrosDirectory(data);
	return true;
}

bool DirectoriesSubscribers::menu(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Directories::SetMenuDirectory(data);
	return true;
}

bool DirectoriesSubscribers::system(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Directories::SetSystemDirectory(data);
	return true;
}

bool DirectoriesSubscribers::web(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Directories::SetWebDirectory(data);
	Model::get().post<EventType::Directories>();
	return true;
}

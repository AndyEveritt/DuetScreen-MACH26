/*
 * FanObservers.cpp
 *
 *  Created on: 8 Jan 2024
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Configuration.h"
#include "FileSubscribers.h"

#include "Comm/FileInfo.h"
#include "Configuration.h"
#include "Hardware/Duet.h"
#include "utils/utils.h"
#include <string>

#include "ObjectModel/Files.h"
#include "UI/Core/Model.h"

bool FileSubscribers::setCurrectDirectory(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	OM::FileSystem::FileListRequestWeakPtr request = OM::FileSystem::GetFileListRequest(data);
	if (!request.lock())
	{
		LOG_WARN("Not expecting to receive file list data for \"{:s}\", ignoring", data);
		return false;
	}

	LOG_DBG("Receiving file list for directory = {:s}", data);
	decoder->responseType = Comm::JsonDecoder::ResponseType::filelist;
	decoder->responseData = request;
	return true;
}

bool FileSubscribers::setFirstIndex(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	if (decoder->responseType != Comm::JsonDecoder::ResponseType::filelist)
	{
		return false;
	}

	auto request = std::get<OM::FileSystem::FileListRequestWeakPtr>(decoder->responseData).lock();
	if (!request)
	{
		return false;
	}

	if (data != request->GetNext())
	{
		LOG_WARN("First index {:d} does not match expected next index {:d}, ignoring", data, request->GetNext());
		decoder->responseType = Comm::JsonDecoder::ResponseType::unknown;
		decoder->responseData = nullptr;
		return false;
	}

	LOG_DBG("First index = {:d}", data);
	if (data == 0)
	{
		request->ClearItems();
	}
	request->SetFirst(data);
	return true;
}

bool FileSubscribers::setType(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	if (decoder->responseType != Comm::JsonDecoder::ResponseType::filelist)
	{
		return false;
	}

	auto request = std::get<OM::FileSystem::FileListRequestWeakPtr>(decoder->responseData).lock();
	if (!request)
	{
		return false;
	}

	LOG_DBG("type check val={:s}", data);
	switch (*data)
	{
	case 'd':
		request->AddFolder();
		break;
	case 'f':
		request->AddFile();
		break;
	}
	return true;
}

bool FileSubscribers::setName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	if (decoder->responseType != Comm::JsonDecoder::ResponseType::filelist)
	{
		return false;
	}

	auto request = std::get<OM::FileSystem::FileListRequestWeakPtr>(decoder->responseData).lock();
	if (!request)
	{
		return false;
	}

	OM::FileSystem::ItemPtr item = request->GetLastItem();
	if (item == nullptr)
		return false;

	LOG_DBG("name = {:s}", data);
	item->SetName(data);
	item->SetPath(request->GetDir());
	return true;
}

bool FileSubscribers::setSize(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	if (decoder->responseType != Comm::JsonDecoder::ResponseType::filelist)
	{
		return false;
	}

	auto request = std::get<OM::FileSystem::FileListRequestWeakPtr>(decoder->responseData).lock();
	if (!request)
	{
		return false;
	}

	OM::FileSystem::ItemPtr item = request->GetLastItem();
	if (item == nullptr)
		return false;

	LOG_DBG("size = {:d}", data);
	item->SetSize(data);
	return true;
}

bool FileSubscribers::setDate(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	if (decoder->responseType != Comm::JsonDecoder::ResponseType::filelist)
	{
		return false;
	}

	auto request = std::get<OM::FileSystem::FileListRequestWeakPtr>(decoder->responseData).lock();
	if (!request)
	{
		return false;
	}

	OM::FileSystem::ItemPtr item = request->GetLastItem();
	if (item == nullptr)
		return false;

	LOG_DBG("date = {:s}", data);
	item->SetDate(data);
	return true;
}

bool FileSubscribers::setNextIndex(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	if (decoder && decoder->GetSeq() != nullptr)
	{
		// There is a key collision with `M409 K"boards" F"v"`, so we need to check if the key is part of an OM request
		return true;
	}

	if (decoder->responseType != Comm::JsonDecoder::ResponseType::filelist)
	{
		return false;
	}

	auto request = std::get<OM::FileSystem::FileListRequestWeakPtr>(decoder->responseData).lock();
	if (!request)
	{
		return false;
	}

	LOG_DBG("next index = {:d}", data);
	request->SetNext(data);
	request->RunCallback();

	if (data == 0)
	{
		return true;
	}
	Comm::DUET.RequestFileList(request->GetDir(), data);
	return true;
}

bool FileSubscribers::setFragFilename(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	OM::FileSystem::FileContentsPtr fileContents = OM::FileSystem::GetCurrentFileRequestContents();
	if (!fileContents || fileContents->GetFilename() != data)
	{
		LOG_WARN("Not expecting to receive file contents data for \"{:s}\", ignoring", data);
		return false;
	}

	decoder->responseType = Comm::JsonDecoder::ResponseType::fileContents;
	decoder->responseData = fileContents;

	LOG_DBG("Receiving file contents information about {}", fileContents->GetFilename());
	return true;
}

static bool getFileContentsFromDecoder(Comm::JsonDecoder* decoder, OM::FileSystem::FileContentsPtr& fileContents)
{
	ZoneScoped;
	try
	{
		fileContents = std::get<OM::FileSystem::FileContentsPtr>(decoder->responseData);
	}
	catch (const std::bad_variant_access&)
	{
		return false;
	}
	if (fileContents == nullptr)
	{
		LOG_ERROR("Not expecting to receive file contents data");
		return false;
	}
	return true;
}

bool FileSubscribers::setFragOffset(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	OM::FileSystem::FileContentsPtr fileContents;
	if (!getFileContentsFromDecoder(decoder, fileContents))
	{
		return false;
	}

	uint32_t offset = 0;
	if (!Comm::GetUnsignedInteger(data, offset))
	{
		LOG_WARN("file contents offset error \"{:s}\"", data);
		fileContents->SetParseError(-1);
		return false;
	}
	fileContents->SetOffset(offset);
	LOG_DBG("file contents receive current offset {:d}.", fileContents->GetOffset());
	return true;
}

bool FileSubscribers::setFragData(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	OM::FileSystem::FileContentsPtr fileContents;
	if (!getFileContentsFromDecoder(decoder, fileContents))
	{
		return false;
	}

	LOG_DBG("file contents data len: {:d}", strlen(data));
	fileContents->AppendData(data);
	return true;
}

bool FileSubscribers::setFragNext(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	OM::FileSystem::FileContentsPtr fileContents;
	if (!getFileContentsFromDecoder(decoder, fileContents))
	{
		return false;
	}

	uint32_t next = 0;
	if (!Comm::GetUnsignedInteger(data, next))
	{
		LOG_WARN("file contents next error \"{:s}\"", data);
		fileContents->SetParseError(-2);
		return false;
	}
	fileContents->SetNext(next);
	LOG_DBG("file contents next {:d}", fileContents->GetNext());
	return true;
}

bool FileSubscribers::setFragErr(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	OM::FileSystem::FileContentsPtr fileContents;
	if (!getFileContentsFromDecoder(decoder, fileContents))
	{
		return false;
	}

	int32_t err = 0;
	if (!Comm::GetInteger(data, err))
	{
		LOG_WARN("Failed to parse file contents err {:s}", data);
		fileContents->SetParseError(-3);
		return false;
	}

	if (err != 0)
	{
		return false;
	}

	fileContents->RunCallback();

	if (fileContents->GetNext() != 0)
	{
		Comm::DUET.SendGcodef("M36.2 P\"{:s}\" S{:d}", fileContents->GetFilename(), fileContents->GetNext());
	}

	return true;
}

bool FileSubscribers::arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	ZoneScoped;
	UNUSED(indices);
	{
		MODEL_LOCK();
		if (decoder->responseType != Comm::JsonDecoder::ResponseType::filelist)
		{
			return false;
		}

		auto request = std::get<OM::FileSystem::FileListRequestWeakPtr>(decoder->responseData).lock();
		if (!request)
		{
			return false;
		}

		/* We don't care about file info (and thumbnails) for files that aren't in the Gcodes directory */
		if (request->GetDir().find(OM::Directories::GetGcodesDirectory()) == std::string::npos)
		{
			return true;
		}

		for (size_t i = 0; i < request->GetItemCount(); i++)
		{
			auto item = request->GetItem(i);
			if (item == nullptr || item->GetType() == OM::FileSystem::FileSystemItemType::folder)
			{
				continue;
			}
			Comm::FileInfoPtr fileInfo = FILEINFO_CACHE->GetFileInfo(item->GetPath());
			if (fileInfo == nullptr || !fileInfo->lastModified.Equals(item->GetDate().c_str()))
			{
				FILEINFO_CACHE->QueueFileInfoRequest(item->GetPath());
			}
			else if (fileInfo && !FILEINFO_CACHE->IsThumbnailCached(item->GetPath(), item->GetDate().c_str()))
			{
				FILEINFO_CACHE->QueueThumbnailRequest(item->GetPath());
			}
		}
	}
	return true;
}
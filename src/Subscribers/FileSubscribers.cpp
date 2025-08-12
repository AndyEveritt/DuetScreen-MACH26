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
	OM::FileSystem::SetCurrentDir(data);
	LOG_DBG("Files: current dir = {:s}", OM::FileSystem::GetCurrentDirPath().c_str());
	decoder->responseType = Comm::JsonDecoder::ResponseType::filelist;
	auto fileData = std::make_shared<Comm::JsonDecoder::FileListData>(OM::FileSystem::GetCurrentDirPath());
	decoder->responseData = fileData;
	return true;
}

bool FileSubscribers::setFirstIndex(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	std::get<Comm::JsonDecoder::FileListDataPtr>(decoder->responseData)->first = data;
	if (data == 0)
	{
		OM::FileSystem::ClearFileSystem();
		Model::get().post<EventType::FileData>();
	}
	return true;
}

bool FileSubscribers::setType(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	LOG_DBG("Files: type check val={:s}", data);
	uint32_t index = indices[0] + std::get<Comm::JsonDecoder::FileListDataPtr>(decoder->responseData)->first;
	switch (*data)
	{
	case 'd':
		OM::FileSystem::AddFolderAt(index);
		LOG_DBG("Files: folder at index {:d}", index);
		break;
	case 'f':
		OM::FileSystem::AddFileAt(index);
		LOG_DBG("Files: file at index {:d}", index);
		break;
	}
	return true;
}

bool FileSubscribers::setName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	auto responseData = std::get<Comm::JsonDecoder::FileListDataPtr>(decoder->responseData);
	uint32_t index = indices[0] + responseData->first;
	LOG_DBG("Files: item[{:d}] name={:s}", index, data);
	std::shared_ptr<OM::FileSystem::FileSystemItem> item = OM::FileSystem::GetItem(index);
	if (item == nullptr)
		return false;

	item->SetName(data);
	item->SetPath(responseData->dir);
	return true;
}

bool FileSubscribers::setSize(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	uint32_t index = indices[0] + std::get<Comm::JsonDecoder::FileListDataPtr>(decoder->responseData)->first;
	LOG_DBG("Files: item[{:d}] size={:d}", index, data);
	std::shared_ptr<OM::FileSystem::FileSystemItem> item = OM::FileSystem::GetItem(index);
	if (item == nullptr)
		return false;
	item->SetSize(data);
	return true;
}

bool FileSubscribers::setDate(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	uint32_t index = indices[0] + std::get<Comm::JsonDecoder::FileListDataPtr>(decoder->responseData)->first;
	LOG_DBG("Files: item[{:d}] date={:s}", index, data);
	std::shared_ptr<OM::FileSystem::FileSystemItem> item = OM::FileSystem::GetItem(index);
	if (item == nullptr)
		return false;
	item->SetDate(data);
	return true;
}

bool FileSubscribers::setNextIndex(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (decoder && decoder->GetSeq() != nullptr)
	{
		// There is a key collision with `M409 K"boards" F"v"`, so we need to check if the key is part of an OM request
		return true;
	}
	LOG_DBG("Files: next index = {:d}", data);

	OM::FileSystem::RunCallback(data);

	if (data == 0)
	{
		return true;
	}
	Comm::DUET.RequestFileList(std::get<Comm::JsonDecoder::FileListDataPtr>(decoder->responseData)->dir.c_str(), data);
	return true;
}

bool FileSubscribers::setFragFilename(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
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
		Comm::DUET.SendGcodef("M36.2 P\"%s\" S%d", fileContents->GetFilename().data(), fileContents->GetNext());
	}

	return true;
}

bool FileSubscribers::arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	{
		MODEL_LOCK();
		OM::FileSystem::SortFileSystem(OM::FileSystem::SortBy::DATE, true);
		for (size_t i = 0; i < OM::FileSystem::GetItemCount(); i++)
		{
			std::shared_ptr<OM::FileSystem::FileSystemItem> item = OM::FileSystem::GetItem(i);
			if (item == nullptr || item->GetType() == OM::FileSystem::FileSystemItemType::folder)
			{
				continue;
			}
			if (item->GetPath().find("gcodes") == std::string::npos)
			{
				continue;
			}
			Comm::FileInfoPtr fileInfo = FILEINFO_CACHE->GetFileInfo(item->GetPath());
			if (fileInfo == nullptr || !fileInfo->lastModified.Equals(item->GetDate().c_str()))
			{
				FILEINFO_CACHE->QueueFileInfoRequest(item->GetPath());
			}
		}
	}
	Model::get().post<EventType::FileData>();
	return true;
}
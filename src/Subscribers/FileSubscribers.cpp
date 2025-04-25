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
	Comm::JsonDecoder::FileListData* fileData =
		new Comm::JsonDecoder::FileListData(OM::FileSystem::GetCurrentDirPath());
	decoder->responseData = fileData;
	return true;
}

bool FileSubscribers::setFirstIndex(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first = data;
	if (data == 0)
	{
		OM::FileSystem::ClearFileSystem();
		Model::get().newFileData();
	}
	return true;
}

bool FileSubscribers::setType(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	LOG_DBG("Files: type check val={:s}", data);
	uint32_t index = indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
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
	uint32_t index = indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
	LOG_DBG("Files: item[{:d}] name={:s}", index, data);
	std::shared_ptr<OM::FileSystem::FileSystemItem> item = OM::FileSystem::GetItem(index);
	if (item == nullptr)
		return false;

	item->SetName(data);
	return true;
}

bool FileSubscribers::setSize(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	uint32_t index = indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
	LOG_DBG("Files: item[{:d}] size={:d}", index, data);
	std::shared_ptr<OM::FileSystem::FileSystemItem> item = OM::FileSystem::GetItem(index);
	if (item == nullptr)
		return false;
	item->SetSize(data);
	return true;
}

bool FileSubscribers::setDate(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	uint32_t index = indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
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
	Comm::DUET.RequestFileList(static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->dir.c_str(), data);
	return true;
}

bool FileSubscribers::arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	{
		MODEL_LOCK();
		OM::FileSystem::SortFileSystem();
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
	Model::get().newFileData();
	return true;
}
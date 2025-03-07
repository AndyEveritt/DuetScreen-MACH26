/*
 * FanSubscribers.cpp
 *
 *  Created on: 4 Jan 2024
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Configuration.h"
#include "Comm/FileInfo.h"
#include "ThumbnailSubscribers.h"
#include "UI/Core/Model.h"

bool ThumbnailSubscribers::fileName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	FILEINFO_CACHE->SetCurrentFileInfo(data);
	dbg("Receiving file info about %s", FILEINFO_CACHE->GetCurrentFileInfo()->filename.c_str());
	return true;
}

bool ThumbnailSubscribers::lastModified(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	info("lastModified %s", data);
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->lastModified.copy(data);
	return true;
}

bool ThumbnailSubscribers::size(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->size = data;
	return true;
}

bool ThumbnailSubscribers::printTime(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->printTime = data;
	return true;
}

bool ThumbnailSubscribers::simulatedTime(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->printTime = data;
	return true;
}

bool ThumbnailSubscribers::height(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->height = data;
	return true;
}

bool ThumbnailSubscribers::layerHeight(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->layerHeight = data;
	return true;
}

bool ThumbnailSubscribers::thumbnailsFormat(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	verbose("thumbnail format %s", data);
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	Comm::Thumbnail* thumbnail = fileInfo->GetOrCreateThumbnail(indices[0]);
	if (!thumbnail->meta.SetImageFormat(data))
	{
		warn("Thumbnail format invalid");
	}
	return true;
}

bool ThumbnailSubscribers::thumbnailsHeight(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	verbose("thumbnail height %d", data);
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->GetOrCreateThumbnail(indices[0])->meta.height = data;
	return true;
}

bool ThumbnailSubscribers::thumbnailsOffset(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	verbose("thumbnail offset %d", data);
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	Comm::Thumbnail* thumbnail = fileInfo->GetOrCreateThumbnail(indices[0]);
	thumbnail->meta.offset = data;
	thumbnail->context.next = data;
	return true;
}

bool ThumbnailSubscribers::thumbnailsSize(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	verbose("thumbnail size %d", data);
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->GetOrCreateThumbnail(indices[0])->meta.size = data;
	return true;
}

bool ThumbnailSubscribers::thumbnailsWidth(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	verbose("thumbnail width %d", data);
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->GetOrCreateThumbnail(indices[0])->meta.width = data;
	return true;
}

bool ThumbnailSubscribers::generatedBy(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
	{
		warn("FileInfo not found");
		return false;
	}
	fileInfo->generatedBy.copy(data);
	FILEINFO_CACHE->FileInfoRequestComplete();
	return true;
}

bool ThumbnailSubscribers::thumbnailFilename(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail(true);
	decoder->responseType = Comm::JsonDecoder::ResponseType::thumbnail;
	decoder->responseData = thumbnail;
	if (thumbnail == nullptr)
	{
		error("Not expecting to receive thumbnail data");
		return false;
	}
	if (!thumbnail->filename.Equals(data))
	{
		warn("M36.1 filename (%s) not the same as what is expected (%s)", data, thumbnail->filename.c_str());
		thumbnail->context.parseErr = -2;
	}
	info("Receiving thumbnail information about %s", thumbnail->filename.c_str());
	FILEINFO_CACHE->ReceivingThumbnailResponse(true);
	return true;
}

bool ThumbnailSubscribers::thumbnailOffset(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
	if (thumbnail == nullptr)
	{
		error("Not expecting to receive thumbnail data");
		return false;
	}

	if (!Comm::GetUnsignedInteger(data, thumbnail->context.offset))
	{
		warn("thumbnail offset error \"%s\"", data);
		thumbnail->context.parseErr = -4;
		return false;
	}
	dbg("thumbnail receive current offset %u.", thumbnail->context.offset);
	return true;
}

bool ThumbnailSubscribers::thumbnailData(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
	if (thumbnail == nullptr)
	{
		error("Not expecting to receive thumbnail data");
		return false;
	}

	dbg("thumbnail data %d", strlen(data));
	Comm::g_thumbnailBuf.size = strnlen(data, sizeof(Comm::g_thumbnailBuf.buffer));
	memcpy(Comm::g_thumbnailBuf.buffer, data, Comm::g_thumbnailBuf.size);
	thumbnail->context.state = Comm::ThumbnailState::Data;
	return true;
}

bool ThumbnailSubscribers::thumbnailNext(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
	if (thumbnail == nullptr)
	{
		error("Not expecting to receive thumbnail data");
		return false;
	}

	if (!Comm::GetUnsignedInteger(data, thumbnail->context.next))
	{
		warn("thumbnail next error \"%s\"", data);
		thumbnail->context.parseErr = -4;
		return false;
	}
	dbg("thumbnail next %u", thumbnail->context.next);
	return true;
}

bool ThumbnailSubscribers::thumbnailErr(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
	if (thumbnail == nullptr)
	{
		error("Not expecting to receive thumbnail data");
		return false;
	}

	if (!Comm::GetInteger(data, thumbnail->context.err))
	{
		warn("Failed to parse thumbnail err %s", data);
		thumbnail->context.parseErr = -1;
	}
	info("Thumbnail: offset(%d), next(%d), err(%d), size(%d), parseErr(%d)",
		 thumbnail->context.offset,
		 thumbnail->context.next,
		 thumbnail->context.err,
		 thumbnail->context.size,
		 thumbnail->context.parseErr);
	return true;
}

bool ThumbnailSubscribers::thumbnailsArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	dbg("Thumbnail array end");
	Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
	if (fileInfo == nullptr)
		return false;
	fileInfo->ClearThumbnails(indices[0]);
	FILEINFO_CACHE->QueueThumbnailRequest(fileInfo->filename.c_str());
	info("FileInfo: filename(%s) thumbnails(%d)", fileInfo->filename.c_str(), fileInfo->GetThumbnailCount());
	for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
	{
		Comm::Thumbnail* thumbnail = fileInfo->GetOrCreateThumbnail(i);
		info("Thumbnail %d: filename(%s) offset(%d) size(%d) width(%d) height(%d) format(%d)",
			 i,
			 thumbnail->filename.c_str(),
			 thumbnail->meta.offset,
			 thumbnail->meta.size,
			 thumbnail->meta.width,
			 thumbnail->meta.height,
			 thumbnail->meta.imageFormat);
	}
	return true;
}

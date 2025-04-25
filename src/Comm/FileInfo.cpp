/*
 * FileInfo.cpp
 *
 *  Created on: 12 Mar 2024
 *      Author: andy
 */

#include "Debug.h"
#include "DebugCommands.h"

#include "FileInfo.h"

#include "Configuration.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/PrinterStatus.h"
#include "UI/Core/Model.h"
#include "utils/utils.h"
#include <sys/stat.h>
#include <utils/TimeHelper.h>

namespace Comm
{
	FileInfo::FileInfo()
		: size(0)
		, height(0)
		, layerHeight(0)
		, printTime(0)
	{
		LOG_DBG("Created new fileinfo");
	}

	FileInfo::~FileInfo()
	{
		LOG_DBG("Deleted fileinfo {:s}", filename.c_str());
	}

	std::shared_ptr<Thumbnail> FileInfo::GetThumbnail(size_t index)
	{
		if (index >= m_thumbnails.size())
		{
			return nullptr;
		}

		return m_thumbnails[index];
	}

	std::shared_ptr<Thumbnail> FileInfo::GetOrCreateThumbnail(size_t index)
	{
		if (index >= m_thumbnails.size())
		{
			m_thumbnails.resize(index + 1);
			m_thumbnails[index] = std::make_shared<Thumbnail>(filename.GetRef());
		}

		return m_thumbnails[index];
	}

	size_t FileInfo::ClearThumbnails(size_t fromIndex)
	{
		size_t count = m_thumbnails.size() - fromIndex;
		m_thumbnails.resize(fromIndex);
		return count;
	}

	tm FileInfo::GetPrintTime() const
	{
		tm time = ParseSeconds(printTime);
		LOG_DBG("Print time ({:d}): {:d}:{:2d}:{:2d}", printTime, time.tm_hour, time.tm_min, time.tm_sec);
		return time;
	}

	FileInfoCache::FileInfoCache() {}

	void FileInfoCache::Spin()
	{
		int64_t now = TimeHelper::getCurrentTime();

		// Timeout any request that hasn't received a response within the timeout period
		for (auto it = m_fileInfoRequestQueue.begin(); it != m_fileInfoRequestQueue.end();)
		{
			FileInfoRequest& request = *it;
			it++;

			if (request.HasTimedOut(FILE_CACHE_REQUEST_TIMEOUT))
			{
				LOG_WARN("File info request timed out for {:s}", request.GetData()->filename.c_str());
				request.Complete(true);
				LOG_WARN("Requeuing failed file info request for {:s}", request.GetData()->filename.c_str());
				QueueFileInfoRequest(request.GetData()->filename.c_str());
			}
		}

		for (auto it = m_thumbnailRequestQueue.begin(); it != m_thumbnailRequestQueue.end();)
		{
			ThumbnailRequest& request = *it;
			it++;

			if (!ThumbnailIsValid(*request.GetData()))
			{
				LOG_ERROR("Invalid thumbnail");
				m_thumbnailRequestQueue.remove(request);
				continue;
			}

			if (request.HasTimedOut(FILE_CACHE_REQUEST_TIMEOUT))
			{
#if DEBUG
				ThumbnailPtr t = request.GetData();
#endif
				LOG_WARN("Thumbnail request timed out for {:s}", request.GetData()->filename.c_str());
				request.Complete(true);
				LOG_WARN("Requeuing thumbnail request for {:s}", request.GetData()->filename.c_str());
				std::string filename = request.GetData()->filename.c_str();
				DeleteCachedThumbnail(filename.c_str());
				QueueThumbnailRequest(filename);
			}
		}

		if ((OM::PrintInProgress()) && (now - m_lastRequestTime < BACKGROUND_FILE_CACHE_POLL_INTERVAL))
		{
			LOG_VERBOSE("Skipping file info cache spin");
			return;
		}

		// Start a new request if there are no requests in progress
		size_t fileInfoRequested = 0;
		for (FileInfoRequest& request : m_fileInfoRequestQueue)
		{
			if (request.IsRequested())
			{
				fileInfoRequested++;
				continue;
			}

			if (fileInfoRequested >= MAX_FILEINFO_REQUESTS)
			{
				break;
			}

			request.RequestData();
			fileInfoRequested++;
		}

		// Start a new thumbnail request if there are none in progress
		size_t thumbnailsRequested = 0;
		for (auto it = m_thumbnailRequestQueue.begin(); it != m_thumbnailRequestQueue.end();)
		{
			ThumbnailRequest& request = *it;
			ThumbnailPtr thumbnail = request.GetData();
			if (thumbnail == nullptr)
			{
				// Should be impossible
				LOG_ERROR("Null thumbnail");
				continue;
			}

			++it;

			switch (thumbnail->context.state)
			{
			case ThumbnailState::Init:
			case ThumbnailState::DataRequest:
				if (!request.RequestData())
				{
				}
				thumbnailsRequested++;
				break;
			case ThumbnailState::Data:
			case ThumbnailState::DataWait:
				LOG_VERBOSE("Thumbnail request in progress for {:s}, state={}",
							thumbnail->filename.c_str(),
							thumbnail->context.state);
				thumbnailsRequested++;
				break;
			case ThumbnailState::Cached:
				thumbnail->image.Close();
				LOG_DBG("Updating thumbnail {:s}", thumbnail->filename.c_str());
				ThumbnailRequestComplete(thumbnail->filename.c_str());
				Model::get().newThumbnailData(thumbnail->filename.c_str());
				break;
			default:
				break;
			}

			if (thumbnailsRequested >= MAX_THUMBNAIL_REQUESTS)
			{
				break;
			}
		}
// Check if a request has finished
#if 0
		if (m_thumbnailRequestInProgress)
		{
			if (m_currentThumbnail->context.parseErr != 0 || m_currentThumbnail->context.err != 0)
			{
				LOG_WARN("Thumbnail request failed for {:s}, parseErr({:d}), err({:d})",
					 m_currentThumbnail->filename.c_str(),
					 m_currentThumbnail->context.parseErr,
					 m_currentThumbnail->context.err);
				DeleteCachedThumbnail(m_currentThumbnail->filename.c_str());
				m_thumbnailRequestInProgress = false;
				m_currentThumbnail = nullptr;
				return;
			}

			// Check if the request is done
			switch (m_currentThumbnail->context.state)
			{
			case ThumbnailState::Init:
				m_currentThumbnail = nullptr;
				m_thumbnailRequestInProgress = false;
				m_thumbnailResponseInProgress = false;
				break;
			case ThumbnailState::Data:
			case ThumbnailState::DataWait:
				LOG_VERBOSE("Thumbnail request in progress for {:s}, state={:d}",
						m_currentThumbnail->filename.c_str(),
						m_currentThumbnail->context.state);
				return;
			case ThumbnailState::DataRequest:
				m_lastThumbnailRequestTime = TimeHelper::getCurrentTime();
				DUET.RequestThumbnail(m_currentThumbnail->filename.c_str(), m_currentThumbnail->context.next);
				m_currentThumbnail->context.state = ThumbnailState::DataWait;
				return;
			case ThumbnailState::Cached:
				m_currentThumbnail->image.Close();
				LOG_DBG("Updating thumbnail {:s}", m_currentThumbnail->filename.c_str());
#  if 0
				UI::FileList::GetThumbnail()->setText("");
				UI::GetUIControl<ZKListView>(ID_MAIN_FileListView)->refreshListView();
				if (m_currentThumbnail->AboveCacheLimit())
				{
					UI::POPUP_WINDOW.SetImage(GetThumbnailPath(largeThumbnailFilename).c_str());
				}
#  endif
				if (m_currentThumbnail->filename.Equals(OM::GetJobName().c_str()))
				{
					if (GetFileSize(currentJobThumbnailFilePath) <
						GetFileSize(m_currentThumbnail->GetThumbnailPath().c_str()))
					{
						system(utils::format("cp %s %s",
											 m_currentThumbnail->GetThumbnailPath().c_str(),
											 currentJobThumbnailFilePath)
								   .c_str());
#  if 0
						UI::GetUIControl<ZKTextView>(ID_MAIN_PrintThumbnail)
							->setBackgroundPic(currentJobThumbnailFilePath);
#  endif
					}
					m_currentCachedJobPath = OM::GetJobName();
				}
				m_thumbnailRequestInProgress = false;
				m_currentThumbnail = nullptr;
				break;
			default:
				break;
			}
		}

		if (!OM::GetJobName().empty() && m_currentCachedJobPath != OM::GetJobName())
		{
			// Set the thumbnail to a small version if it exists
#  if 0
			UI::GetUIControl<ZKTextView>(ID_MAIN_PrintThumbnail)
				->setBackgroundPic(GetThumbnailPath(OM::GetJobName().c_str()).c_str());
#  endif

			// Queue a request for a large thumbnail
			bool queued = QueueLargeThumbnailRequest(OM::GetJobName());
			if (GetFileInfo(OM::GetJobName()) != nullptr && !queued)
			{
				// No valid thumbnail
				m_currentCachedJobPath = OM::GetJobName();
			}
		}

		if (m_queuedLargeThumbnail != nullptr)
		{
			LOG_INFO("Requesting queued large thumbnail for {:s}", m_queuedLargeThumbnail->filename.c_str());
			if (RequestThumbnail(m_queuedLargeThumbnail))
			{
				m_queuedLargeThumbnail = nullptr;
				return;
			}
		}

		if (!m_fileInfoRequestQueue.empty() && !m_fileInfoRequestInProgress)
		{
			std::string filepath = m_fileInfoRequestQueue.front();
			m_fileInfoRequestQueue.pop_front();
			m_fileInfoRequestInProgress = true;
			m_currentFileInfoRequest = filepath;
			m_lastFileInfoRequestTime = TimeHelper::getCurrentTime();
			DUET.RequestFileInfo(filepath.c_str());
			return;
		}

		if (m_thumbnailRequestQueue.empty())
		{
			// LOG_DBG("Request queue is empty");
			return;
		}

		LOG_DBG("Processing thumbnail request queue");
		Thumbnail* thumbnail = m_thumbnailRequestQueue.front();
		m_thumbnailRequestQueue.pop_front();

		RequestThumbnail(thumbnail);
#endif
	}

	bool FileInfoCache::IsThumbnailCached(const std::string& filepath, const char* lastModified)
	{
		MODEL_LOCK();
		// Does a thumbnail file exist in the file system?
		if (!::IsThumbnailCached(filepath.c_str(), false))
		{
			LOG_DBG("Thumbnail file for {:s} does not exist", filepath.c_str());
			return false;
		}

		// Do we have a cache for the files meta data?
		if (m_cache.find(filepath) == m_cache.end())
		{
			LOG_DBG("No file info cached for {:s}", filepath.c_str());
			return false;
		}

		FileInfoPtr fileInfo = m_cache[filepath];

		// Is the last modified time the same?
		if (!fileInfo->lastModified.Equals(lastModified))
		{
			LOG_DBG("Last modified time for {:s} does not match", filepath.c_str());
			return false;
		}

		return true;
	}

	/**
	 * @brief Get file info for the given gcode file path
	 * @param filepath
	 * @return shared_ptr to FileInfo object
	 */
	FileInfoPtr FileInfoCache::GetFileInfo(const std::string& filepath)
	{
		MODEL_LOCK();
		if (m_cache.find(filepath) == m_cache.end())
		{
			return nullptr;
		}
		return m_cache[filepath];
	}

	bool FileInfoCache::FileInfoRequestInProgress()
	{
		MODEL_LOCK();
		return std::find_if(m_fileInfoRequestQueue.begin(),
							m_fileInfoRequestQueue.end(),
							[](const FileInfoRequest& request)
							{ return request.IsInProgress(); }) != m_fileInfoRequestQueue.end();
	}

	void FileInfoCache::ReceivingFileInfoResponse(const std::string& filepath)
	{
		MODEL_LOCK();
		FileInfoRequest* request = GetFileInfoRequest(filepath);
		if (request == nullptr)
		{
			request = &m_fileInfoRequestQueue.emplace_front(filepath);
		}

		request->Receiving();
	}

	/**
	 * @brief Get the FileInfoRequest object for the given filepath
	 * @param filepath
	 * @param createIfNotFound
	 * @return
	 */
	FileInfoCache::FileInfoRequest* FileInfoCache::GetFileInfoRequest(const std::string& filepath)
	{
		MODEL_LOCK();
		for (FileInfoRequest& request : m_fileInfoRequestQueue)
		{
			if (request.GetData()->filename.Equals(filepath.c_str()))
			{
				return &request;
			}
		}
		return nullptr;
	}

	bool FileInfoCache::IsFileInfoRequestQueued(const std::string& filepath)
	{
		MODEL_LOCK();
		return GetFileInfoRequest(filepath) != nullptr;
	}

	bool FileInfoCache::IsFileInfoRequestInProgress(const std::string& filepath)
	{
		MODEL_LOCK();
		FileInfoRequest* request = GetFileInfoRequest(filepath);
		if (request == nullptr)
		{
			return false;
		}

		return request->IsInProgress();
	}

	void FileInfoCache::FileInfoRequestComplete(const std::string& filepath)
	{
		LOG_DBG("File info request complete for {:s}", filepath.c_str());

		FileInfoRequest* request = GetFileInfoRequest(filepath);

		if (request == nullptr)
		{
			return;
		}

		request->Complete();
		m_cache[filepath] = request->GetData();

		m_fileInfoRequestQueue.remove(*request);
	}

	bool FileInfoCache::FileInfoRequest::RequestDataInner()
	{
		if (m_data == nullptr)
		{
			return false;
		}

		LOG_DBG("Requesting file info for \"{:s}\", ", m_data->filename.c_str());
		return DUET.RequestFileInfo(m_data->filename.c_str());
	}

	bool FileInfoCache::ThumbnailRequest::RequestDataInner()
	{
		if (m_data == nullptr)
		{
			return false;
		}

		LOG_DBG("Requesting thumbnail for \"{:s}\", {:d}x{:d}",
				m_data->filename.c_str(),
				m_data->meta.width,
				m_data->meta.height);

		if (m_data->filename.IsEmpty() || m_data->meta.offset == 0)
		{
			LOG_WARN("Not enough information to request thumbnail for {:s}", m_data->filename.c_str());
			return false;
		}

		if (!ThumbnailIsValid(*m_data))
		{
			LOG_ERROR("thumbnail meta invalid.\n");
			return false;
		}

		if (m_data->context.state == ThumbnailState::DataRequest)
		{
			m_data->context.state = ThumbnailState::DataWait;
			return DUET.RequestThumbnail(m_data->filename.c_str(), m_data->context.next);
		}
		else
		{
			m_data->context.Init();

			const char* filename = m_data->AboveCacheLimit() ? largeThumbnailFilename : m_data->filename.c_str();
			if (!m_data->image.New(m_data->meta, filename))
			{
				LOG_ERROR("Failed to create thumbnail file {:s}.", filename);
				return false;
			}
			m_data->context.state = ThumbnailState::DataWait;
			return DUET.RequestThumbnail(m_data->filename.c_str(), m_data->meta.offset);
		}
	}

	void FileInfoCache::ClearCache()
	{
		LOG_INFO("Clearing file info cache");

		m_cache.clear();
		m_fileInfoRequestQueue.clear();
		m_thumbnailRequestQueue.clear();
		ClearAllCachedThumbnails();
		LOG_INFO("Cache cleared");
	}

	/**
	 * @brief Queue a file info request for processing
	 * @param filepath The filepath to request information for
	 * @param next If true, try to queue it at the next position
	 * @return True if the request was successfully queued, false otherwise
	 */
	bool FileInfoCache::QueueFileInfoRequest(const std::string& filepath, bool next)
	{
		MODEL_LOCK();
		LOG_DBG("Attempting to queue file info request for {:s}", filepath.c_str());
		for (FileInfoRequest& request : m_fileInfoRequestQueue)
		{
			if (request.GetData()->filename.Equals(filepath.c_str()))
			{
				if (request.IsInProgress())
				{
					// Request already in progress so don't remove it from queue or add it again
					LOG_WARN("File info request for {:s} already in progress", filepath.c_str());
					return false;
				}

				if (!next && !request.IsFailed())
				{
					// Request is already in the queue but has not started
					LOG_DBG("File info request for {:s} already queued", filepath.c_str());
					return false;
				}

				// Request in the queue but not in progress or at the front
				m_fileInfoRequestQueue.remove(request);
				break;
			}
		}
		LOG_DBG("Queueing file info request for {:s}", filepath.c_str());
		if (next)
		{

			auto it = std::find_if(m_fileInfoRequestQueue.begin(),
								   m_fileInfoRequestQueue.end(),
								   [](const FileInfoRequest& request) { return !request.IsInProgress(); });
			m_fileInfoRequestQueue.insert(it, filepath);
		}
		else
		{
			m_fileInfoRequestQueue.push_back(filepath);
		}
		return true;
	}

	bool FileInfoCache::QueueThumbnailRequest(const std::string& filepath, bool next)
	{
		MODEL_LOCK();
		LOG_DBG("Attempting to queue thumbnail request for {:s}", filepath.c_str());
		for (ThumbnailRequest& request : m_thumbnailRequestQueue)
		{
			if (request.GetData()->filename.Equals(filepath.c_str()))
			{
				if (request.IsInProgress())
				{
					// Request already in progress so don't remove it from queue or add it again
					LOG_WARN("Thumbnail request for {:s} already in progress", filepath.c_str());
					return false;
				}

				if (!next && !request.IsFailed())
				{
					// Request is already in the queue but has not started
					LOG_DBG("Thumbnail request for {:s} already queued", filepath.c_str());
					return false;
				}

				// Request in the queue but not in progress or at the front
				m_thumbnailRequestQueue.remove(request);
				break;
			}
		}

		FileInfoPtr fileInfo = GetFileInfo(filepath);
		if (fileInfo == nullptr)
		{
			LOG_DBG("No file info found for {:s}", filepath.c_str());
			FileInfoRequest* request = GetFileInfoRequest(filepath);
			if (request == nullptr)
			{
				LOG_WARN("Request not in progress for \"{:s}\", queuing file info request", filepath.c_str());
				QueueFileInfoRequest(filepath, next);
				return false;
			}

			if (!request->IsReceiving())
			{
				LOG_WARN("FileInfo request for \"{:s}\" has not been received yet, not queuing thumbnail request",
						 filepath.c_str());
				return false;
			}

			fileInfo = request->GetData();
		}

		ThumbnailPtr largestValidThumbnail;
		size_t largestSize = 0;
		for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
		{
			ThumbnailPtr thumbnail = fileInfo->GetThumbnail(i);
			if (thumbnail == nullptr)
				continue;
			if (!thumbnail->AboveCacheLimit() && thumbnail->meta.width * thumbnail->meta.height > largestSize)
			{
				largestValidThumbnail = thumbnail;
				largestSize = thumbnail->meta.width * thumbnail->meta.height;
			}
		}
		if (largestValidThumbnail == nullptr)
		{
			LOG_WARN("No valid thumbnail found for {:s}", filepath.c_str());
			return false;
		}

		largestValidThumbnail->context.Init();
		if (next)
		{
			auto it = std::find_if(m_thumbnailRequestQueue.begin(),
								   m_thumbnailRequestQueue.end(),
								   [](const ThumbnailRequest& request) { return !request.IsInProgress(); });
			m_thumbnailRequestQueue.insert(it, largestValidThumbnail);
		}
		else
		{
			m_thumbnailRequestQueue.push_back(largestValidThumbnail);
		}
		LOG_DBG("Queued thumbnail request for \"{:s}\", {:d}x{:d}",
				filepath.c_str(),
				largestValidThumbnail->meta.width,
				largestValidThumbnail->meta.height);
		return true;
	}

	bool FileInfoCache::QueueLargeThumbnailRequest(const std::string& filepath)
	{
		MODEL_LOCK();
#if 0
		m_queuedLargeThumbnail = nullptr;
		DeleteCachedThumbnail(largeThumbnailFilename);

		FileInfo* fileInfo = GetFileInfo(filepath);
		if (fileInfo == nullptr)
		{
			QueueFileInfoRequest(filepath, true);
			return false;
		}

		Thumbnail* largestThumbnail = nullptr;
		LayoutPosition pos = UI::FileList::GetThumbnail()->getPosition();
		int closestDistance = INT_MAX;
		for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
		{
			Thumbnail* thumbnail = fileInfo->GetThumbnail(i);
			if (thumbnail == nullptr || !thumbnail->AboveCacheLimit())
				continue;

			int xDiff = (int)thumbnail->meta.width - pos.mWidth;
			int yDiff = (int)thumbnail->meta.height - pos.mHeight;
			int distance = xDiff * xDiff + yDiff * yDiff;

			if (distance < closestDistance)
			{
				largestThumbnail = thumbnail;
				closestDistance = distance;
			}
		}
		if (largestThumbnail == nullptr)
		{
			LOG_WARN("No valid thumbnail found for {:s}", filepath.c_str());
			return false;
		}
		LOG_INFO("Queued large thumbnail request for {:s}, size={:d}, offset={:d}",
			 filepath.c_str(),
			 largestThumbnail->meta.size,
			 largestThumbnail->meta.offset);
		m_queuedLargeThumbnail = largestThumbnail;
#endif
		return true;
	}

	bool FileInfoCache::RequestThumbnail(FileInfo& fileInfo, size_t index)
	{
		MODEL_LOCK();
		if (index >= fileInfo.GetThumbnailCount())
		{
			LOG_WARN("Thumbnail index {:d} out of range for {:s}", index, fileInfo.filename.c_str());
			return false;
		}

		ThumbnailPtr thumbnail = fileInfo.GetThumbnail(index);

		return RequestThumbnail(thumbnail);
	}

	bool FileInfoCache::RequestThumbnail(ThumbnailPtr thumbnail)
	{
		MODEL_LOCK();
#if 0
		if (thumbnail == nullptr)
		{
			LOG_WARN("Thumbnail is null");
			return false;
		}

		thumbnail->context.Init();
		if (thumbnail->filename.IsEmpty() || thumbnail->meta.offset == 0)
		{
			LOG_WARN("Not enough information to request thumbnail for {:s}", thumbnail->filename.c_str());
			return false;
		}

		if (!ThumbnailIsValid(*thumbnail))
		{
			LOG_ERROR("thumbnail meta invalid.\n");
			return false;
		}

		const char* filename = thumbnail->AboveCacheLimit() ? largeThumbnailFilename : thumbnail->filename.c_str();

		if (!thumbnail->image.New(thumbnail->meta, filename))
		{
			LOG_ERROR("Failed to create thumbnail file {:s}.", filename);
			return false;
		}

		ThumbnailRequest* request = GetThumbnailRequest(thumbnail->filename.c_str());
		thumbnail->context.state = ThumbnailState::DataWait;
		DUET.RequestThumbnail(thumbnail->filename.c_str(), thumbnail->meta.offset);
#endif
		return true;
	}

	ThumbnailPtr FileInfoCache::GetRequestedThumbnail(const std::string& filepath)
	{
		MODEL_LOCK();
		auto it = std::find_if(m_thumbnailRequestQueue.begin(),
							   m_thumbnailRequestQueue.end(),
							   [filepath](const ThumbnailRequest& request)
							   { return request.GetData()->filename.Equals(filepath.c_str()); });
		if (it != m_thumbnailRequestQueue.end())
		{
			return it->GetData();
		}
		return nullptr;
	}

	bool FileInfoCache::ThumbnailRequestInProgress()
	{
		MODEL_LOCK();
		return std::find_if(m_thumbnailRequestQueue.begin(),
							m_thumbnailRequestQueue.end(),
							[](const ThumbnailRequest& request)
							{ return request.IsInProgress(); }) != m_thumbnailRequestQueue.end();
	}

	void FileInfoCache::ThumbnailRequestComplete(const std::string& filepath)
	{
		MODEL_LOCK();
		ThumbnailRequest* request = GetThumbnailRequest(filepath);
		if (request == nullptr)
		{
			return;
		}

		request->Complete();
		m_thumbnailRequestQueue.remove(*request);
	}

	FileInfoCache::ThumbnailRequest* FileInfoCache::GetThumbnailRequest(const std::string& filepath)
	{
		MODEL_LOCK();
		for (ThumbnailRequest& request : m_thumbnailRequestQueue)
		{
			if (request.GetData()->filename.Equals(filepath.c_str()))
			{
				return &request;
			}
		}
		return nullptr;
	}

	/**
	 * @brief Stops the current thumbnail request. Will not stop a thumbnail request if it is for the current print job.
	 * @param largeOnly If true, only stops the request if the current thumbnail is above the cache limit.
	 * @return True if the thumbnail request was stopped, false otherwise.
	 */
	bool FileInfoCache::StopThumbnailRequest(bool largeOnly)
	{
		MODEL_LOCK();
		return true;
	}

	void FileInfoCache::Debug()
	{
		MODEL_LOCK();
		LOG_DBG("File info cache debug");
		printf("File info cache:");
		for (auto& it : get()->m_cache)
		{
			FileInfoPtr fileInfo = it.second;
			if (fileInfo == nullptr)
				continue;
			printf("  File %s:", fileInfo->filename.c_str());
			printf("    size: %u", fileInfo->size);
			printf("    lastModified: %s", fileInfo->lastModified.c_str());
			printf("    height: %.3f", fileInfo->height);
			printf("    layerHeight: %.3f", fileInfo->layerHeight);
			printf("    thumbnails: %lu", fileInfo->GetThumbnailCount());

			for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
			{
				ThumbnailPtr thumbnail = fileInfo->GetThumbnail(i);
				if (thumbnail == nullptr)
					continue;
				printf("    Thumbnail %lu:", i);
				printf("      meta:");
				printf("        filename: %s", thumbnail->filename.c_str());
				printf("        width(%u), height(%u), format(%d), offset(%u), size(%u)",
					   thumbnail->meta.width,
					   thumbnail->meta.height,
					   thumbnail->meta.imageFormat,
					   thumbnail->meta.offset,
					   thumbnail->meta.size);
				printf("      context:");
				printf("        err(%d), parseErr(%d), size(%u), offset(%u), next(%u), state(%d)",
					   thumbnail->context.err,
					   thumbnail->context.parseErr,
					   thumbnail->context.size,
					   thumbnail->context.offset,
					   thumbnail->context.next,
					   thumbnail->context.state);
			}
		}

		printf("  File info request queue:");
		for (FileInfoRequest& request : m_fileInfoRequestQueue)
		{
			printf("    %s", request.GetData()->filename.c_str());
		}

		printf("  Thumbnail request queue:");
		for (ThumbnailRequest request : m_thumbnailRequestQueue)
		{
			printf("    %dx%d %s",
				   request.GetData()->meta.width,
				   request.GetData()->meta.height,
				   request.GetData()->filename.c_str());
		}
	}

	tm ParseSeconds(uint32_t seconds)
	{
		tm time;
		time.tm_hour = static_cast<int>(seconds / 3600);
		time.tm_min = static_cast<int>((seconds - time.tm_hour * 3600) / 60);
		time.tm_sec = static_cast<int>(seconds - time.tm_hour * 3600 - time.tm_min * 60);
		return time;
	}

	size_t GetFileSize(const char* filepath)
	{
		MODEL_LOCK();
		struct stat sb;
		if (system(utils::format("test -f \"%s\"", filepath).c_str()) == 0)
		{
			if (stat(filepath, &sb) == -1)
			{
				// File doesn't exist
				return 0;
			}
			return sb.st_size;
		}
		return 0;
	}

	static Debug::DebugCommand s_dbgFileInfoCache("dbg_file_info_cache", []() { FileInfoCache::get()->Debug(); });
} // namespace Comm

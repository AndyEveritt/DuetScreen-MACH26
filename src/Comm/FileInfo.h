/*
 * FileInfo.h
 *
 *  Created on: 12 Mar 2024
 *      Author: andy
 */

#ifndef JNI_COMM_FILEINFO_H_
#define JNI_COMM_FILEINFO_H_

#include "Thumbnail.h"

#include "Comm/Commands.h"
#include "Configuration.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/StringRef.h"
#include "utils/TimeHelper.h"
#include <list>
#include <map>
#include <memory>
#include <stdint.h>
#include <vector>

namespace Comm
{
	constexpr const char* largeThumbnailFilename = "largeThumbnail";
	constexpr const char* currentJobThumbnailFilePath = "/tmp/currentJobThumbnail";

	using ThumbnailPtr = std::shared_ptr<Thumbnail>;

	struct FileInfo
	{
	  public:
		FileInfo();
		~FileInfo();

		String<MAX_FILENAME_LENGTH> filename;
		uint32_t size = 0;
		String<19> lastModified;
		float height = 0;
		float layerHeight = 0;
		uint32_t printTime = 0;
		std::vector<float> filament;
		String<64> generatedBy;

		ThumbnailPtr GetThumbnail(size_t index);
		ThumbnailPtr GetOrCreateThumbnail(size_t index);
		size_t GetThumbnailCount() const { return m_thumbnails.size(); }
		size_t ClearThumbnails(size_t fromIndex);

		tm GetPrintTime() const;
		std::string GetReadableFileSize() const;

	  private:
		std::vector<ThumbnailPtr> m_thumbnails;
	};

	using FileInfoPtr = std::shared_ptr<FileInfo>;

	enum class RequestState
	{
		UNKNOWN = 0,
		QUEUED,
		REQUESTED,
		RECEIVING,
		COMPLETE,
		FAILED
	};

	class FileInfoCache
	{
	  public:
		template <typename T>
		struct Request
		{

			Request()
				: m_data(std::make_shared<T>())
			{
			}

			Request(const std::shared_ptr<T>& data)
				: m_data(data)
			{
			}

			std::shared_ptr<T> GetData() const { return m_data; }
			RequestState GetState() const { return m_state; }

			bool IsRequested() const { return m_state >= RequestState::REQUESTED; }
			bool IsInProgress() const
			{
				return m_state == RequestState::REQUESTED || m_state == RequestState::RECEIVING;
			}
			bool IsReceiving() const { return m_state == RequestState::RECEIVING; }
			bool IsComplete() const { return m_state == RequestState::COMPLETE || m_state == RequestState::FAILED; }
			bool IsFailed() const { return m_state == RequestState::FAILED; }

			void Receiving() { m_state = RequestState::RECEIVING; }
			void Complete(bool failed = false) { m_state = failed ? RequestState::FAILED : RequestState::COMPLETE; }

			int64_t GetRequestTime() const { return m_requestTime; }
			bool HasTimedOut(uint32_t timeout) const
			{
				return m_state == RequestState::REQUESTED && TimeHelper::getTimeSince(m_requestTime) > timeout;
			}

			void RequestData()
			{
				m_requestTime = TimeHelper::getCurrentTime();
				if (!RequestDataInner())
				{
					return;
				}
				m_state = RequestState::REQUESTED;
			}

			bool operator==(const Request& other) { return m_data == other.m_data; }

		  protected:
			virtual bool RequestDataInner() = 0;
			void SetState(RequestState state) { m_state = state; }

			std::shared_ptr<T> m_data;
			RequestState m_state = RequestState::UNKNOWN;
			int64_t m_requestTime = 0;
		};

		struct FileInfoRequest : public Request<FileInfo>
		{
			FileInfoRequest(const std::string& filepath)
				: Request<FileInfo>()
			{
				m_data->filename.copy(filepath.c_str());
			}

		  protected:
			bool RequestDataInner() override;
		};

		struct ThumbnailRequest : public Request<Thumbnail>
		{
			using Request<Thumbnail>::Request;
			ThumbnailBuf& GetBuffer() { return m_buf; }

		  protected:
			bool RequestDataInner() override;

			ThumbnailBuf m_buf;
		};

		static FileInfoCache* get()
		{
			static FileInfoCache instance;
			return &instance;
		}

		void Spin(); // processes thumbnail requests

		bool IsThumbnailCached(const std::string& filepath,
							   const char* lastModified); // returns true if a thumbnail for the file is in cache
		// and lastModified is the same

		FileInfoPtr GetFileInfo(const std::string& filepath);
		void ReceivingFileInfoResponse(const std::string& filepath);
		FileInfoRequest* GetFileInfoRequest(const std::string& filepath);
		void FileInfoRequestComplete(const std::string& filepath); // called when the file info request is complete

		ThumbnailPtr GetRequestedThumbnail(const std::string& filepath);
		bool CancelThumbnailRequest(const std::string& filepath);
		ThumbnailRequest* GetThumbnailRequest(const std::string& filepath);
		void ThumbnailRequestComplete(const std::string& filepath);

		void ClearCache(); // clears the cache

		bool QueueFileInfoRequest(const std::string& filepath, bool next = false);
		bool QueueThumbnailRequest(const std::string& filepath,
								   bool next = false);				  // returns true if the request was queued
		bool QueueLargeThumbnailRequest(const std::string& filepath); // returns true if a thumbnail request was started

		bool StopThumbnailRequest(bool largeOnly = false);

		void Debug(); // prints debug info

	  private:
		FileInfoCache();

		bool FileInfoRequestInProgress();
		bool ThumbnailRequestInProgress();

		bool IsFileInfoRequestQueued(const std::string& filepath);
		bool IsFileInfoRequestInProgress(const std::string& filepath);

		bool IsThumbnailRequestQueued(const std::string& filepath);
		bool IsThumbnailRequestInProgress(const std::string& filepath);

		bool RequestFileInfo(const std::string& filepath);
		bool RequestThumbnail(FileInfo& fileInfo, size_t index);
		bool RequestThumbnail(ThumbnailPtr thumbnail);

		ThumbnailPtr GetNextThumbnail();

		std::map<std::string, FileInfoPtr> m_cache; // cache of file path and their associated file info
		std::list<FileInfoRequest> m_fileInfoRequestQueue;
		std::list<ThumbnailRequest> m_thumbnailRequestQueue;
		int64_t m_lastRequestTime = 0;
	};

	tm ParseSeconds(uint32_t seconds);
	size_t GetFileSize(const char* filepath);
} // namespace Comm
#define FILEINFO_CACHE Comm::FileInfoCache::get()

#endif /* JNI_COMM_FILEINFO_H_ */

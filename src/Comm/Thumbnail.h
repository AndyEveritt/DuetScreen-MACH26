#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP 1

#include <cstddef>
#include <sys/types.h>

#  include "Configuration.h"
#  include "Duet3D/General/String.h"
#  include "image/bmp.h"
#  include "image/png.h"
#  include "image/qoi.h"
#  include <nlohmann/json.hpp>
#  include <string>

namespace Comm
{
	enum ThumbnailState
	{
		Init = 0,
		DataRequest,
		DataWait,
		Data,
		Cached
	};

	struct ThumbnailMeta
	{
		uint32_t width;
		uint32_t height;
		enum ImageFormat
		{
			Invalid = 0,
			Qoi,
			Png,
		} imageFormat;
		uint32_t offset;
		uint32_t size;

		bool SetImageFormat(std::string_view format);
	};

	void to_json(nlohmann::json& j, const ThumbnailMeta& m);
	void from_json(const nlohmann::json& j, ThumbnailMeta& m);

	struct ThumbnailContext
	{
		enum ThumbnailState state;
		int16_t parseErr;
		int32_t err;
		uint32_t size;
		uint32_t offset;
		uint32_t next;

		ThumbnailContext() { Init(); }

		void Init()
		{
			state = ThumbnailState::Init;
			parseErr = 0;
			err = 0;
			size = 0;
			offset = 0;
			next = 0;
		};
	};

	struct ThumbnailImage
	{
		uint32_t pixel_count;

		qoi_desc qoi;
		PNG png;
		BMP bmp;
		std::string imageFilename;

		bool New(ThumbnailMeta& meta, std::string_view filename);
		bool IsOpen() const;
		bool Close();
	};

	struct Thumbnail
	{
		StringRef filename;
		ThumbnailMeta meta;
		ThumbnailContext context;
		ThumbnailImage image;

		Thumbnail(StringRef name)
			: filename(name)
		{
		}
		uint32_t GetProgress() const;
		bool AboveCacheLimit() const;
		std::string GetThumbnailPath() const;
	};

	void to_json(nlohmann::json& j, const Thumbnail& t);
	void from_json(const nlohmann::json& j, Thumbnail& t);

	struct ThumbnailBuf
	{
		uint16_t size;
		unsigned char buffer[MAX_JSON_VALUE_LENGTH]; // 1024 bytes for UART, 2600 bytes for network, some extra since we
													 // have lots of memory. DSF will send the thumbnail in 1 chunk
	};
} // namespace Comm

typedef bool (*ThumbnailProcessCb)(const struct Thumbnail& thumbnail,
								   uint32_t pixels_offset,
								   const rgba_t* pixels,
								   size_t pixels_count);

bool ThumbnailIsValid(Comm::Thumbnail& thumbnail);
bool ThumbnailDataIsValid(Comm::ThumbnailBuf& data);

int ThumbnailInit(Comm::Thumbnail& thumbnail);
int ThumbnailDecodeChunk(Comm::Thumbnail& thumbnail, Comm::ThumbnailBuf& data);

std::string GetThumbnailPath(std::string_view filepath);
bool IsThumbnailCached(std::string_view filepath, bool includeBlank = false);
bool ClearAllCachedThumbnails();
bool DeleteCachedThumbnail(std::string_view filepath);
bool CreateBlankThumbnailCache(std::string_view filepath);

#endif /* ifndef THUMBNAIL_HPP */

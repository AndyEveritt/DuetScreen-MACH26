#pragma once

#include "Subscribers/Subscribers.h"

class ThumbnailSubscribers : public SubscriberMap
{
  public:
	ThumbnailSubscribers()
	{
		addSubscriber("fileName", fileName);
		addSubscriber("lastModified", lastModified);
		addSubscriber("size", size);
		addSubscriber("printTime", printTime);
		addSubscriber("simulatedTime", simulatedTime);
		addSubscriber("height", height);
		addSubscriber("layerHeight", layerHeight);
		addSubscriber("filament^", filament);
		addSubscriber("thumbnails^:format", thumbnailsFormat);
		addSubscriber("thumbnails^:height", thumbnailsHeight);
		addSubscriber("thumbnails^:offset", thumbnailsOffset);
		addSubscriber("thumbnails^:size", thumbnailsSize);
		addSubscriber("thumbnails^:width", thumbnailsWidth);
		addSubscriber("generatedBy", generatedBy);
		addSubscriber("thumbnail:fileName", thumbnailFilename);
		addSubscriber("thumbnail:offset", thumbnailOffset);
		addSubscriber("thumbnail:data", thumbnailData);
		addSubscriber("thumbnail:next", thumbnailNext);
		addSubscriber("thumbnail:err", thumbnailErr);

		addArrayEndSubscriber("thumbnails^", thumbnailsArrayEnd);
	}

  private:
	static bool fileName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool lastModified(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool size(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool printTime(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool simulatedTime(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool height(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool layerHeight(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool filament(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool thumbnailsFormat(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool thumbnailsHeight(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool thumbnailsOffset(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool thumbnailsSize(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool thumbnailsWidth(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool generatedBy(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool thumbnailFilename(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool thumbnailOffset(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool thumbnailData(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool thumbnailNext(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool thumbnailErr(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);

	static bool thumbnailsArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
};
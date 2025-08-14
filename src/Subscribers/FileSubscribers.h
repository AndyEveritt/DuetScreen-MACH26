#pragma once

#include "Subscribers/Subscribers.h"

class FileSubscribers : public SubscriberMap
{
  public:
	FileSubscribers()
	{
		addSubscriber("dir", setCurrectDirectory);
		addSubscriber("first", setFirstIndex);
		addSubscriber("files^:type", setType);
		addSubscriber("files^:name", setName);
		addSubscriber("files^:size", setSize);
		addSubscriber("files^:date", setDate);
		addSubscriber("next", setNextIndex);

		addSubscriber("fragment:fileName", setFragFilename);
		addSubscriber("fragment:offset", setFragOffset);
		addSubscriber("fragment:data", setFragData);
		addSubscriber("fragment:next", setFragNext);
		addSubscriber("fragment:err", setFragErr);

		addArrayEndSubscriber("files^", arrayEnd);
	}

  private:
	static bool setCurrectDirectory(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setFirstIndex(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool setType(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setSize(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool setDate(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setNextIndex(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool setFragFilename(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setFragOffset(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setFragData(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setFragNext(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool setFragErr(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
};

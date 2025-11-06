#pragma once

#include "Subscribers/Subscribers.h"

enum class ResponseType
{
	INFO = 0,
	SUCCESS,
	WARNING,
	ERROR,
	NONE
};

constexpr std::string_view RESPONSE_TYPE_STRINGS[] = {"settings.severity.info",
													  "settings.severity.success",
													  "settings.severity.warn",
													  "settings.severity.error",
													  "settings.severity.none"};

static_assert(std::size(RESPONSE_TYPE_STRINGS) == static_cast<size_t>(ResponseType::NONE) + 1,
			  "RESPONSE_TYPE_STRINGS size does not match ResponseType enum");

class ResponseSubscribers : public SubscriberMap
{
  public:
	ResponseSubscribers()
	{
		addSubscriber("resp", resp);
		addSubscriber("message", message);
		addSubscriber("seq", seq);
		addSubscriber("seqs:reply", seqReply);
	}

  private:
	static bool resp(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool message(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool seq(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool seqReply(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
};

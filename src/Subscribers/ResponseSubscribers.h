#pragma once

#include "Subscribers/Subscribers.h"
#include <magic_enum/magic_enum.hpp>

enum class ResponseType
{
	INFO = 0,
	SUCCESS,
	WARNING,
	ERROR,
};

constexpr std::string_view RESPONSE_TYPE_STRINGS[] = {
	"settings.severity.info",
	"settings.severity.success",
	"settings.severity.warn",
	"settings.severity.error",
};

static_assert(std::size(RESPONSE_TYPE_STRINGS) == magic_enum::enum_count<ResponseType>(),
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

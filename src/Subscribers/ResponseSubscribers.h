#pragma once

#include "Subscribers/Subscribers.h"

enum class ResponseType
{
	SUCCESS = 0,
	INFO,
	WARNING,
	ERROR
};

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

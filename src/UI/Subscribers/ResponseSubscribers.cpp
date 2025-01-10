#include "Debug.h"

#include "Hardware/Duet.h"
#include "ResponseSubscribers.h"
#include "UI/Core/model.h"

bool ResponseSubscribers::resp(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	info("resp length=%d", strlen(data));
	dbg("resp: %s", data);
	static std::string str;
	size_t substrlen;
	str = data;
	// TODO add line to vector of responses

	Model::get().newResponse();

	return true;
}

bool ResponseSubscribers::message(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	info("%s", data);
	return true;
}

bool ResponseSubscribers::seq(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	info("%s", data);
	return true;
}

bool ResponseSubscribers::seqReply(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	static int32_t lastSeq = -1;

	if (data == lastSeq)
	{
		return true;
	}

	lastSeq = data;
	info("%d", data);
	if (Comm::DUET.GetCommunicationType() == Comm::CommunicationType::network)
	{
		info("New reply available");
		HttpResponse r;
		Comm::DUET.RequestReply(r);
		Comm::DUET.ProcessReply(r);
	}
	return true;
}

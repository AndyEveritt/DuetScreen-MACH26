#include "Debug.h"

#include "Hardware/Duet.h"
#include "ResponseSubscribers.h"
#include "UI/Core/Model.h"

bool ResponseSubscribers::resp(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	LOG_INFO("resp length={:d}", strlen(data));
	LOG_DBG("resp: {:s}", data);

	Model::get().post<EventType::Response>(std::string(data));

	return true;
}

bool ResponseSubscribers::message(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	LOG_INFO("{:s}", data);
	return true;
}

bool ResponseSubscribers::seq(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	LOG_INFO("{:s}", data);
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
	LOG_INFO("{:d}", data);
	if (Comm::DUET.GetCommunicationType() == Comm::CommunicationType::network)
	{
		LOG_INFO("Requesting reply for seq {:d}", data);
		HttpResponse r;
		Comm::DUET.RequestReply(r);
		Comm::DUET.ProcessReply(r);
	}
	return true;
}

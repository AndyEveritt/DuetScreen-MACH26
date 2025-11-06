#include "Debug.h"

#include "Hardware/Duet.h"
#include "ResponseSubscribers.h"
#include "UI/Core/Model.h"

bool ResponseSubscribers::resp(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	size_t len = strlen(data);
	LOG_INFO("resp length={:d}", len);
	LOG_DBG("resp: {:s}", data);
	if (data == nullptr)
	{
		return false;
	}

	ResponseType type = ResponseType::INFO;

	std::string trimmed;
	if (len > 0 && (data[len - 1] == '\n' || data[len - 1] == '\r'))
	{
		size_t end = len;
		while (end > 0 && (data[end - 1] == '\n' || data[end - 1] == '\r'))
		{
			--end;
		}
		trimmed.assign(data, end);
	}
	else
	{
		trimmed = data;
	}

	if (trimmed.empty())
	{
		return false;
	}

	if (trimmed.starts_with("Error: "))
	{
		type = ResponseType::ERROR;
	}
	else if (trimmed.starts_with("Warning: "))
	{
		type = ResponseType::WARNING;
	}

	Model::get().post<EventType::Response>(type, trimmed);
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

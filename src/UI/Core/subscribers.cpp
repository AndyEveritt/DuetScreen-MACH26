#include "subscribers.h"

std::map<const char*, std::vector<Subscriber>, ConstCharComparator> SubscriberMap::m_subscribers;

size_t SubscriberMap::getSubscriberCount(const char* key)
{
	auto it = m_subscribers.find(key);
	if (it == m_subscribers.end())
	{
		return 0;
	}
	return it->second.size();
}

const std::vector<Subscriber>& SubscriberMap::getSubscribers(const char* key)
{
	auto it = m_subscribers.find(key);
	if (it->second.size() == 0)
	{
		static std::vector<Subscriber> empty;
		return empty;
	}
	return it->second;
}

void SubscriberMap::addSubscriber(const char* key, subscriberCb_t cb)
{
	if (key == nullptr)
		return;

	m_subscribers[key].emplace_back(key, cb);
}

void SubscriberMap::addSubscriber(const char* key,
								  bool (*cb)(Comm::JsonDecoder* decoder, const float& val, const size_t indices[]))
{
	addSubscriber(key,
				  [cb](Comm::JsonDecoder* decoder, const char* data, const size_t arrayIndices[])
				  {
					  float val = 0;
					  if (Comm::GetFloat(data, val))
					  {
						  return cb(decoder, val, arrayIndices);
					  }
					  return false;
				  });
}

void SubscriberMap::addSubscriber(const char* key,
								  bool (*cb)(Comm::JsonDecoder* decoder, const uint32_t& val, const size_t indices[]))
{
	addSubscriber(key,
				  [cb](Comm::JsonDecoder* decoder, const char* data, const size_t arrayIndices[])
				  {
					  uint32_t val = 0;
					  if (Comm::GetUnsignedInteger(data, val))
					  {
						  return cb(decoder, val, arrayIndices);
					  }
					  return false;
				  });
}

void SubscriberMap::addSubscriber(const char* key,
								  bool (*cb)(Comm::JsonDecoder* decoder, const int32_t& val, const size_t indices[]))
{
	addSubscriber(key,
				  [cb](Comm::JsonDecoder* decoder, const char* data, const size_t arrayIndices[])
				  {
					  int32_t val = 0;
					  if (Comm::GetInteger(data, val))
					  {
						  return cb(decoder, val, arrayIndices);
					  }
					  return false;
				  });
}

void SubscriberMap::addSubscriber(const char* key,
								  bool (*cb)(Comm::JsonDecoder* decoder, const bool& val, const size_t indices[]))
{
	addSubscriber(key,
				  [cb](Comm::JsonDecoder* decoder, const char* data, const size_t arrayIndices[])
				  {
					  bool val = 0;
					  if (Comm::GetBool(data, val))
					  {
						  return cb(decoder, val, arrayIndices);
					  }
					  return false;
				  });
}

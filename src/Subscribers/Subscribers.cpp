#include "Subscribers.h"
#include "Debug.h"

SubscriberMap::SubscriberMap_t SubscriberMap::s_subscribers;
SubscriberMap::ArrayEndSubscriberMap_t SubscriberMap::s_arrayEndSubscribers;

size_t const SubscriberMap::getSubscriberCount(const char* key)
{
	auto it = s_subscribers.find(key);
	if (it == s_subscribers.end())
	{
		return 0;
	}
	return it->second.size();
}

const std::vector<Subscriber>& SubscriberMap::getSubscribers(const char* key)
{
	auto it = s_subscribers.find(key);
	if (it == s_subscribers.end())
	{
		static std::vector<Subscriber> empty;
		return empty;
	}
	return it->second;
}

size_t const SubscriberMap::getArrayEndSubscriberCount(const char* key)
{
	auto it = s_arrayEndSubscribers.find(key);
	if (it == s_arrayEndSubscribers.end())
	{
		return 0;
	}
	return it->second.size();
}

const std::vector<ArrayEndSubscriber>& SubscriberMap::getArrayEndSubscribers(const char* key)
{
	auto it = s_arrayEndSubscribers.find(key);
	if (it == s_arrayEndSubscribers.end())
	{
		static std::vector<ArrayEndSubscriber> empty;
		return empty;
	}
	return it->second;
}

void SubscriberMap::_addSubscriber(const char* key, subscriberCb_t cb)
{
	if (key == nullptr)
		return;

	LOG_VERBOSE(fmt::format("Adding subscriber for key '{:s}' cb @ {}", key, static_cast<const void*>(&cb)));

	s_subscribers[key].emplace_back(key, cb);
}

void SubscriberMap::addSubscriber(const char* key,
								  bool (*cb)(Comm::JsonDecoder* decoder, const char* val, const size_t indices[]))
{
	_addSubscriber(key, cb);
}
void SubscriberMap::addSubscriber(const char* key,
								  bool (*cb)(Comm::JsonDecoder* decoder, const float& val, const size_t indices[]))
{
	_addSubscriber(key,
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
	_addSubscriber(key,
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
	_addSubscriber(key,
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
	_addSubscriber(key,
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

void SubscriberMap::addArrayEndSubscriber(const char* key,
										  bool (*cb)(Comm::JsonDecoder* decoder, const size_t indices[]))
{
	// Check key ends with '^', otherwise raise compiler error
	if (key[strlen(key) - 1] != '^')
	{
		LOG_ERROR("Key '{:s}' must end with '^' as it is provided as an array end subscriber");
		return;
	}

	LOG_VERBOSE("Adding array end subscriber for key '{:s}' cb @ {:p}", key, static_cast<const void*>(&cb));
	s_arrayEndSubscribers[key].emplace_back(key, cb);
}

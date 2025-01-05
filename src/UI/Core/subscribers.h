#pragma once

#include "Comm/Communication.h"
#include "Comm/JsonDecoder.h"
#include "ObjectModel/Utils.h"
#include <functional>
#include <map>

#define SUBSCRIBER_ARGS Comm::JsonDecoder *decoder, const char *data, const size_t indices[]

typedef std::function<bool(SUBSCRIBER_ARGS)> subscriberCb_t;

class Subscriber
{
  public:
	Subscriber(const char* key, subscriberCb_t cb)
		: m_key(key)
		, m_cb(cb)
	{
	}

	const char* getKey() const { return m_key; }
	bool run(SUBSCRIBER_ARGS) { return m_cb(decoder, data, indices); }

  private:
	const char* m_key;
	subscriberCb_t m_cb;
};

class SubscriberMap
{
	typedef std::map<const char*, std::vector<Subscriber>, ConstCharComparator> SubscriberMap_t;

  public:
	static size_t getSubscriberCount(const char* key);
	static const std::vector<Subscriber>& getSubscribers(const char* key);

	// Add a callback to be run when json matching key is received
	static void addSubscriber(const char* key,
							  bool (*cb)(Comm::JsonDecoder* decoder, const char* val, const size_t indices[]));
	static void addSubscriber(const char* key,
							  bool (*cb)(Comm::JsonDecoder* decoder, const float& val, const size_t indices[]));
	static void addSubscriber(const char* key,
							  bool (*cb)(Comm::JsonDecoder* decoder, const uint32_t& val, const size_t indices[]));
	static void addSubscriber(const char* key,
							  bool (*cb)(Comm::JsonDecoder* decoder, const int32_t& val, const size_t indices[]));
	static void addSubscriber(const char* key,
							  bool (*cb)(Comm::JsonDecoder* decoder, const bool& val, const size_t indices[]));

	static void addSubscriber(const char* key, bool (*cb)(Comm::JsonDecoder* decoder, const size_t indices[]));

  protected:
  private:
	static void addSubscriber(const char* key, subscriberCb_t cb);
	static SubscriberMap_t s_subscribers;
};

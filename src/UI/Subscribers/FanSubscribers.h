#pragma once

#include "UI/Core/subscribers.h"

class FanSubscribers : public SubscriberMap
{
  public:
	FanSubscribers()
	{
		addSubscriber("fans^:actualValue", actualValue);
		addSubscriber("fans^:requestedValue", requestedValue);
		addSubscriber("fans^:rpm", rpm);

		addArrayEndSubscriber("fans^", arrayEnd);
	}

  private:
	static bool actualValue(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool requestedValue(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool rpm(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
};
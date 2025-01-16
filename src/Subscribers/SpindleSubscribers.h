#pragma once

#include "Subscribers/Subscribers.h"

class SpindleSubscribers : public SubscriberMap
{
  public:
	SpindleSubscribers()
	{
		addSubscriber("spindles^", nullSpindle);
		addSubscriber("spindles^:active", activeSpeed);
		addSubscriber("spindles^:canReverse", canReverse);
		addSubscriber("spindles^:current", currentSpeed);
		addSubscriber("spindles^:max", maxSpeed);
		addSubscriber("spindles^:min", minSpeed);
		addSubscriber("spindles^:state", state);

		addArrayEndSubscriber("spindles^", spindleArrayEnd);
	}

  private:
	static bool nullSpindle(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool activeSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool canReverse(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[]);
	static bool currentSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool maxSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool minSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool state(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool spindleArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
};

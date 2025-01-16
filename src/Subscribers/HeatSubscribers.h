#pragma once

#include "Subscribers/Subscribers.h"

class HeatSubscribers : public SubscriberMap
{
  public:
	HeatSubscribers()
	{
		addSubscriber("heat:heaters^", nullHeater);
		addSubscriber("heat:bedHeaters^", bedHeater);
		addSubscriber("heat:chamberHeaters^", chamberHeater);
		addSubscriber("heat:heaters^:current", heaterCurrentTemperature);
		addSubscriber("heat:heaters^:active", heaterActiveTemperature);
		addSubscriber("heat:heaters^:standby", heaterStandbyTemperature);
		addSubscriber("heat:heaters^:avgPwm", heaterAvgPwm);
		addSubscriber("heat:heaters^:min", heaterMinTemperature);
		addSubscriber("heat:heaters^:max", heaterMaxTemperature);
		addSubscriber("heat:heaters^:sensor", heaterSensorNum);
		addSubscriber("heat:heaters^:state", heaterState);

		addArrayEndSubscriber("heat:heaters^", heaterArrayEnd);
		addArrayEndSubscriber("heat:bedHeaters^", bedHeaterArrayEnd);
		addArrayEndSubscriber("heat:chamberHeaters^", chamberHeaterArrayEnd);
	}

  private:
	static bool nullHeater(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool bedHeater(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool chamberHeater(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool heaterCurrentTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool heaterActiveTemperature(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool heaterStandbyTemperature(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool heaterAvgPwm(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool heaterMinTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool heaterMaxTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool heaterSensorNum(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool heaterState(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool heaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
	static bool bedHeaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
	static bool chamberHeaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
};

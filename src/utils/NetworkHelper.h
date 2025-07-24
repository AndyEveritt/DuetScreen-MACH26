/*
 * NetworkHelper.h
 *
 *  Created on: 2025-01-30
 *      Author: Andy Everitt
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct WiFiNetwork
{
	std::string ssid;
	int32_t signal_level;
	int32_t id = -1;
	bool connected = false;

	void clear()
	{
		ssid.clear();
		signal_level = 0;
		id = -1;
		connected = false;
	}
};

namespace NetworkHelper
{
	void enable(bool enable);
	bool isEnabled();
	void reconfigure();
	std::string getIpAddress();
	std::vector<WiFiNetwork> getKnownWiFiNetworks();
	std::vector<WiFiNetwork> scanWiFiNetworks();
	void connect(const std::string& ssid);
	void connect(const std::string& ssid, std::string_view password);
	bool isNetworkKnown(const std::string& ssid);
	void disconnect();
	void reconnect();
	void forgetNetwork(const std::string& ssid);
} // namespace NetworkHelper

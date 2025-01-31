/*
 * NetworkHelper.cpp
 *
 *  Created on: 2025-01-30
 *      Author: Andy Everitt
 */

#include "NetworkHelper.h"
#include "Debug.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace NetworkHelper
{
	static constexpr const char* s_scanCommand = "iw dev wlan0 scan";
	static constexpr const char* s_signal = "signal:";
	static constexpr const char* s_dBm = " dBm";
	static constexpr const char* s_ssid = "SSID:";
	static constexpr const char* s_wpa_supplicant = "/etc/wpa_supplicant.conf";

	static int32_t getNetworkId(const std::string& ssid)
	{
		std::vector<WiFiNetwork> networks = getKnownWiFiNetworks();
		for (const WiFiNetwork& network : networks)
		{
			if (network.ssid == ssid)
			{
				return network.id;
			}
		}
		return -1;
	}

	static void connectToNetworkById(int32_t id)
	{
		std::string command = "wpa_cli select_network " + std::to_string(id);
		int32_t errorCode = system(command.c_str());
		if (errorCode != 0)
		{
			error("Failed to connect to network id=%d, code=%d", id, errorCode);
		}
	}

	static void save()
	{
		int32_t errorCode = system("wpa_cli save_config");
		if (errorCode != 0)
		{
			error("Failed to save wpa_supplicant configuration, code=%d", errorCode);
		}
	}

	void enable(bool enable)
	{
		int32_t errorCode = system(enable ? "ip link set wlan0 up" : "ip link set wlan0 down");
		if (errorCode != 0)
		{
			error("Failed to %s WiFi, code=%d", enable ? "enable" : "disable", errorCode);
		}
	}

	void reconfigure()
	{
		int32_t errorCode = system("wpa_cli reconfigure");
		if (errorCode != 0)
		{
			error("Failed to reconfigure wpa_supplicant, code=%d", errorCode);
		}
	}

	std::vector<WiFiNetwork> getKnownWiFiNetworks()
	{
		std::vector<WiFiNetwork> networks;
		reconfigure();

		FILE* pipe = popen("wpa_cli list_networks", "r");
		if (!pipe)
		{
			error("Failed to run command: wpa_cli list_networks");
			return networks;
		}

		char buffer[256];
		std::string output;
		while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
		{
			output += buffer;
		}
		pclose(pipe);

		std::istringstream stream(output);
		std::string line;
		while (std::getline(stream, line))
		{
			if (line.find("network id") != std::string::npos)
			{
				continue; // Skip the header line
			}

			std::istringstream lineStream(line);
			std::string id, ssid, bssid, flags;
			if (lineStream >> id >> ssid >> bssid >> flags)
			{
				WiFiNetwork network;
				network.ssid = ssid;
				network.id = atoi(id.c_str());
				networks.push_back(network);
			}
		}
		return networks;
	}

	std::vector<WiFiNetwork> scanWiFiNetworks()
	{
		std::vector<WiFiNetwork> knownNetworks = getKnownWiFiNetworks();
		info("Found %d known networks", knownNetworks.size());

		info("Scanning for WiFi networks");
		std::vector<WiFiNetwork> networks;
		FILE* pipe = popen(s_scanCommand, "r"); // Replace wlan0 with your interface
		if (!pipe)
		{
			error("Failed to run command: %s", s_scanCommand);
			return networks;
		}

		char buffer[256];
		std::string output;
		while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
		{
			output += buffer;
		}
		pclose(pipe);

		std::istringstream stream(output);
		std::string line;
		WiFiNetwork network;

		while (std::getline(stream, line))
		{
			if (line.find(s_signal) != std::string::npos)
			{
				size_t start = line.find(s_signal) + strlen(s_signal);
				size_t end = line.find(s_dBm);
				network.signal_level = atoi(line.substr(start, end - start).c_str());
			}
			else if (line.find(s_ssid) != std::string::npos)
			{
				size_t start = line.find(s_ssid) + strlen(s_ssid);
				size_t end = line.length();
				network.ssid = line.substr(start, end - start);
				if (network.ssid.c_str()[0] != '\0')
				{
					network.id = -1;
					for (const WiFiNetwork& knownNetwork : knownNetworks)
					{
						if (network.ssid == knownNetwork.ssid)
						{
							network.id = knownNetwork.id;
							break;
						}
					}
					info("Found network: %s, signal: %d dBm, id: %d",
						 network.ssid.c_str(),
						 network.signal_level,
						 network.id);
					networks.push_back(network);
				}
			}
		}

		info("Sorting networks by signal level");
		std::sort(networks.begin(),
				  networks.end(),
				  [](const WiFiNetwork& a, const WiFiNetwork& b) { return a.signal_level > b.signal_level; });
		return networks;
	}

	bool isNetworkKnown(const std::string& ssid)
	{
		std::ifstream wpa_supplicant(s_wpa_supplicant);
		if (!wpa_supplicant.is_open())
		{
			error("Failed to open wpa_supplicant.conf");
			return false;
		}

		std::string line;
		while (std::getline(wpa_supplicant, line))
		{
			if (line.find("ssid=\"" + ssid + "\"") != std::string::npos)
			{
				return true;
			}
		}

		return false;
	}

	void connect(const std::string& ssid)
	{
		int32_t id = getNetworkId(ssid);
		if (id == -1)
		{
			error("Failed to get network id for \"%s\"", ssid.c_str());
			return;
		}
		connectToNetworkById(id);
	}

	void connect(const std::string& ssid, const std::string& password)
	{
		if (!isNetworkKnown(ssid))
		{
			std::string command = "wpa_passphrase " + ssid + " " + password + " >>" + s_wpa_supplicant;
			int32_t errorCode = system(command.c_str());
			if (errorCode != 0)
			{
				error("Failed to save details for network \"%s\", code=%d", ssid.c_str(), errorCode);
				return;
			}
		}
		int32_t id = getNetworkId(ssid);
		if (id == -1)
		{
			error("Failed to get network id for \"%s\"", ssid.c_str());
			return;
		}
		connectToNetworkById(id);
	}

	void disconnect()
	{
		int32_t errorCode = system("wpa_cli disconnect");
	}

	void reconnect()
	{
		int32_t errorCode = system("wpa_cli reconnect");
		if (errorCode != 0)
		{
			error("Failed to restart wpa_supplicant, code=%d", errorCode);
		}
	}

	void forgetNetwork(const std::string& ssid)
	{
		int32_t id = getNetworkId(ssid);
		if (id == -1)
		{
			error("Failed to get network id for \"%s\"", ssid.c_str());
			return;
		}
		std::string command = "wpa_cli remove_network " + std::to_string(id);
		int32_t errorCode = system(command.c_str());
		if (errorCode != 0)
		{
			error("Failed to forget network \"%s\", code=%d", ssid.c_str(), errorCode);
			return;
		}
		save();
		reconfigure();
	}
} // namespace NetworkHelper
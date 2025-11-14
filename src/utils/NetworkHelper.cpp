/*
 * NetworkHelper.cpp
 *
 *  Created on: 2025-01-30
 *      Author: Andy Everitt
 */

#include "NetworkHelper.h"
#include "Debug.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#if T113
#  include <wpa_ctrl.h>
#endif

namespace NetworkHelper
{
#define INTERFACE "wlan0"
#define CTRL_PATH "/var/run/wpa_supplicant"
#define WPA_SUPPLICANT_CONF "/etc/wpa_supplicant.conf"

	static constexpr int s_timeout_ms = 10000;

	[[maybe_unused]] static struct wpa_ctrl* s_ctrl_conn = nullptr;
	[[maybe_unused]] static struct wpa_ctrl* s_monitor_conn = nullptr;
	static std::vector<WiFiNetwork> s_networks;

	[[maybe_unused]] static bool initWPAControl()
	{
		LOG_DBG("Initializing wpa_supplicant control interface");
#if T113
		if (s_ctrl_conn != nullptr)
			return true;

		std::string ctrl_path = CTRL_PATH "/" INTERFACE;
		s_ctrl_conn = wpa_ctrl_open(ctrl_path.c_str());
		if (s_ctrl_conn == nullptr)
		{
			LOG_ERROR("Failed to connect to wpa_supplicant");
			return false;
		}

		s_monitor_conn = wpa_ctrl_open(ctrl_path.c_str());
		if (s_monitor_conn == nullptr)
		{
			wpa_ctrl_close(s_ctrl_conn);
			s_ctrl_conn = nullptr;
			LOG_ERROR("Failed to open monitor connection");
			return false;
		}

		if (wpa_ctrl_attach(s_monitor_conn) != 0)
		{
			wpa_ctrl_close(s_monitor_conn);
			wpa_ctrl_close(s_ctrl_conn);
			s_monitor_conn = nullptr;
			s_ctrl_conn = nullptr;
			LOG_ERROR("Failed to attach to wpa_supplicant");
			return false;
		}

		return true;
#else
		return false;
#endif
	}

	[[maybe_unused]] static void closeWPAControl()
	{
		LOG_DBG("Closing wpa_supplicant control interface");
#if T113
		if (s_monitor_conn != nullptr)
		{
			wpa_ctrl_detach(s_monitor_conn);
			wpa_ctrl_close(s_monitor_conn);
			s_monitor_conn = nullptr;
		}
		if (s_ctrl_conn != nullptr)
		{
			wpa_ctrl_close(s_ctrl_conn);
			s_ctrl_conn = nullptr;
		}
#endif
	}

	static std::string sendCommand(const std::string& cmd)
	{
#if T113
		if (!initWPAControl())
		{
			LOG_ERROR("Failed to initialize wpa_supplicant control interface, could not send command: {:s}", cmd);
			return "";
		}

		LOG_INFO("Sending command: {:s}", cmd);
		char buf[4096];
		size_t len = sizeof(buf) - 1;

		int ret = wpa_ctrl_request(s_ctrl_conn, cmd.c_str(), cmd.length(), buf, &len, nullptr);
		if (ret < 0)
		{
			LOG_ERROR("Failed to send command: {:s}", cmd);
			return "";
		}

		buf[len] = '\0';
		return std::string(buf);
#else
		LOG_DBG("Simulating sending command: {:s}", cmd);
		return "";
#endif
	}

	void enable(bool enable)
	{
		LOG_INFO("{:s} WiFi", enable ? "Enabling" : "Disabling");
#if T113
		std::string cmd = fmt::format("ip link set " INTERFACE " {:s}", (enable ? " up" : " down"));
		int32_t errorCode = system(cmd.c_str());
		if (errorCode != 0)
		{
			LOG_ERROR("Failed to {:s} WiFi, code={:d}", enable ? "enable" : "disable", errorCode);
			return;
		}

		if (!std::filesystem::exists(CTRL_PATH "/" INTERFACE))
		{
			errorCode = system("wpa_supplicant -B -i " INTERFACE " -c " WPA_SUPPLICANT_CONF);
			if (errorCode != 0)
			{
				LOG_ERROR("Failed to start wpa_supplicant, code={:d}", errorCode);
				return;
			}
		}
#endif
	}

	bool isEnabled()
	{
		std::string output = sendCommand("STATUS");
		LOG_DBG("WiFi status: {:s}", output);
		return output.find("wpa_state=COMPLETED") != std::string::npos;
	}

	void reconfigure()
	{
		LOG_INFO("Reconfiguring wpa_supplicant");
		sendCommand("RECONFIGURE");
	}

	std::string getIpAddress()
	{
		std::string result;
		std::string output = sendCommand("STATUS");

		size_t pos = output.find("ip_address=");
		if (pos != std::string::npos)
		{
			size_t end = output.find('\n', pos);
			if (end != std::string::npos)
			{
				result = output.substr(pos + 11, end - (pos + 11));
			}
		}
		return result;
	}

	std::vector<WiFiNetwork> getKnownWiFiNetworks()
	{
		LOG_INFO("Getting known WiFi networks");
		std::vector<WiFiNetwork> networks;

		std::string output = sendCommand("LIST_NETWORKS");
		std::istringstream stream(output);
		std::string line;
		bool header = true;

		while (std::getline(stream, line))
		{
			if (header)
			{
				header = false;
				continue;
			}

			std::istringstream iss(line);
			std::string idStr, ssid, bssid, flags;
			if (iss >> idStr >> ssid >> bssid >> flags)
			{
				WiFiNetwork network;
				// Remove quotes if present
				if (ssid.size() >= 2 && ssid.front() == '"' && ssid.back() == '"')
				{
					ssid = ssid.substr(1, ssid.size() - 2);
				}
				network.ssid = ssid;
				network.id = std::stoi(idStr);
				network.connected = (flags.find("[CURRENT]") != std::string::npos);
				networks.push_back(network);
				LOG_INFO("Found known network: \"{:s}\", id: {:d}, current: {:d}",
						 network.ssid.c_str(),
						 network.id,
						 network.connected);
			}
		}
		return networks;
	}

	std::vector<WiFiNetwork> scanWiFiNetworks()
	{
		LOG_INFO("Scanning for WiFi networks");
		std::vector<WiFiNetwork> networks;
		std::vector<WiFiNetwork> knownNetworks = getKnownWiFiNetworks();

		sendCommand("SCAN");
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait 100 milliseconds for scan to complete

		std::string output = sendCommand("SCAN_RESULTS");
		std::istringstream stream(output);
		std::string line;
		bool header = true;

		while (std::getline(stream, line))
		{
			if (header)
			{
				header = false;
				continue;
			}

			std::istringstream iss(line);
			std::string bssid, freq, signal, flags, ssid;
			if (iss >> bssid >> freq >> signal >> flags)
			{
				// Rest of the line is SSID
				std::getline(iss, ssid);
				while (!ssid.empty() && (ssid[0] == ' ' || ssid[0] == '\t'))
				{
					ssid.erase(0, 1);
				}

				if (!ssid.empty())
				{
					WiFiNetwork network;
					network.ssid = ssid;
					network.signal_level = std::stoi(signal);
					network.id = -1;

					// Check if this is a known network
					for (const WiFiNetwork& known : knownNetworks)
					{
						if (network.ssid == known.ssid)
						{
							network.id = known.id;
							network.connected = known.connected;
							break;
						}
					}

					networks.push_back(network);
					LOG_INFO("Found network: \"{:s}\", signal: {:d} dBm, id: {:d}, {:s}",
							 network.ssid.c_str(),
							 network.signal_level,
							 network.id,
							 network.connected ? "connected" : "disconnected");
				}
			}
		}

		// Sort networks by signal strength and connection status
		std::sort(networks.begin(),
				  networks.end(),
				  [](const WiFiNetwork& a, const WiFiNetwork& b)
				  {
					  if (a.connected != b.connected)
						  return a.connected;
					  if (a.id > b.id)
						  return true;
					  return a.signal_level > b.signal_level;
				  });

		return networks;
	}

	bool isNetworkKnown(std::string_view ssid)
	{
		std::vector<WiFiNetwork> networks = getKnownWiFiNetworks();
		return std::any_of(
			networks.begin(), networks.end(), [&ssid](const WiFiNetwork& network) { return network.ssid == ssid; });
	}

	void connect(std::string_view ssid)
	{
		LOG_INFO("Connecting to WiFi network \"{:s}\"", ssid);
		std::vector<WiFiNetwork> networks = getKnownWiFiNetworks();
		for (const WiFiNetwork& network : networks)
		{
			if (network.ssid == ssid)
			{
				std::string cmd = fmt::format("SELECT_NETWORK {}", network.id);
				sendCommand(cmd);
				return;
			}
		}
		LOG_ERROR("Network \"{:s}\" not found in known networks", ssid);
	}

	void connect(std::string_view ssid, std::string_view password)
	{
		LOG_INFO("Connecting to WiFi network \"{:s}\"", ssid);
#if T113
		if (isNetworkKnown(ssid))
		{
			// Network is already known, remove existing entry first
			forgetNetwork(ssid);
		}

		std::string cmd = "ADD_NETWORK";
		std::string output = sendCommand(cmd);
		int networkId = std::stoi(output);

		cmd = fmt::format("SET_NETWORK {:d} ssid \"{:s}\"", networkId, ssid);
		sendCommand(cmd);

		cmd = fmt::format("SET_NETWORK {:d} psk \"{:s}\"", networkId, password);
		sendCommand(cmd);

		cmd = fmt::format("ENABLE_NETWORK {}", networkId);
		sendCommand(cmd);

		sendCommand("SAVE_CONFIG");

		connect(ssid);
#else
		UNUSED(password);
#endif
	}

	void disconnect()
	{
		LOG_INFO("Disconnecting from WiFi network");
		sendCommand("DISCONNECT");
	}

	void reconnect()
	{
		LOG_INFO("Reconnecting to WiFi network");
		sendCommand("RECONNECT");
	}

	void forgetNetwork(std::string_view ssid)
	{
		LOG_INFO("Forgetting network \"{:s}\"", ssid);
		std::vector<WiFiNetwork> networks = getKnownWiFiNetworks();
		for (const WiFiNetwork& network : networks)
		{
			if (network.ssid == ssid)
			{
				std::string cmd = "REMOVE_NETWORK " + std::to_string(network.id);
				sendCommand(cmd);
				sendCommand("SAVE_CONFIG");
				return;
			}
		}
		LOG_ERROR("Network \"{:s}\" not found in known networks", ssid);
	}
} // namespace NetworkHelper
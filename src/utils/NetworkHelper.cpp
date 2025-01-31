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
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace NetworkHelper
{
	static const std::string INTERFACE = "wlan0";
	static constexpr const char* s_scanCommand = "iw dev wlan0 scan";
	static constexpr const char* s_signal = "signal: ";
	static constexpr const char* s_dBm = " dBm";
	static constexpr const char* s_ssid = "SSID: ";
	static constexpr const char* s_wpa_supplicant = "/etc/wpa_supplicant.conf";

	static std::vector<WiFiNetwork> s_networks;

	static std::string readCommandOutput(FILE* pipe)
	{
		constexpr size_t BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];
		std::string output;
		std::vector<char> dynamic_buffer;

		while (true)
		{
			errno = 0;
			if (fgets(buffer, BUFFER_SIZE, pipe) == nullptr)
			{
				// Handle remaining data in dynamic_buffer
				if (!dynamic_buffer.empty())
				{
					output.append(dynamic_buffer.data(), dynamic_buffer.size());
				}

				if (feof(pipe))
					break;
				if (ferror(pipe) && errno == EINTR)
					continue;
				error("Failed to read command output");
				return "";
			}

			// Check if we got a complete line
			size_t len = strlen(buffer);
			if (len > 0 && buffer[len - 1] == '\n')
			{
				output += buffer;
			}
			else
			{
				// Store partial line in dynamic buffer
				dynamic_buffer.insert(dynamic_buffer.end(), buffer, buffer + len);
			}
		}

		return output;
	}

	static std::string executeCommandWithOutput(const std::string& command)
	{
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe)
		{
			error("Failed to run command: %s", command.c_str());
			return "";
		}

		std::string output = readCommandOutput(pipe.get());
		int status = pclose(pipe.release());
		if (WIFEXITED(status))
		{
			if (WEXITSTATUS(status) != 0)
			{
				error("Command failed with exit code: %d", WEXITSTATUS(status));
				return "";
			}
		}
		else
		{
			error("Command terminated abnormally");
			return "";
		}
		return output;
	}

	int executeCommand(const std::string& command, int retries = 3, int delay_ms = 500)
	{
		int result = -1;

		for (int attempt = 0; attempt < retries; ++attempt)
		{
			result = system(command.c_str());
			if (WIFEXITED(result) && WEXITSTATUS(result) == 0)
			{
				return 0;
			}

			if (attempt < retries - 1)
			{
				error("Command failed (attempt %d), retrying in %dms...", attempt + 1, delay_ms);
				usleep(delay_ms * 1000);
				delay_ms *= 2; // Exponential backoff
			}
		}
		return result;
	}

	static std::vector<int> getNetworkIds(const std::string& target_ssid)
	{
		std::vector<int> ids;
		std::string command = "wpa_cli -i " + INTERFACE + " list_networks";
#if SIMULATION
		std::string output = "network id / ssid / bssid / flags\n"
							 "0\t\"MyNetwork\"\tany\t[DISABLED]\n"
							 "1\t\"MyCurrentNetwork\"\t00:00:00:00:00:01\t[CURRENT]\n"
							 "2\t\"MyOtherNetwork\"\t00:00:00:00:00:01\t\n";
#else
		std::string output = executeCommandWithOutput(command);
#endif

		std::istringstream iss(output);
		std::string line;
		bool header = true;

		while (getline(iss, line))
		{
			if (header)
			{
				header = false;
				continue;
			}

			std::vector<std::string> fields;
			std::stringstream lineStream(line);
			std::string field;

			while (getline(lineStream, field, '\t'))
			{
				if (!field.empty())
				{
					fields.push_back(field);
				}
			}

			if (fields.size() >= 4)
			{
				std::string idStr = fields[0];
				std::string ssid = fields[1];

				// Remove quotes from SSID if present
				if (ssid.size() >= 2 && ssid.front() == '"' && ssid.back() == '"')
				{
					ssid = ssid.substr(1, ssid.size() - 2);
				}

				if (ssid == target_ssid)
				{
					try
					{
						ids.push_back(stoi(idStr));
					}
					catch (const std::exception& e)
					{
						error("Invalid network ID: %s", idStr.c_str());
					}
				}
			}
		}
		return ids;
	}

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
		info("Connecting to network id=%d", id);
		std::string command = "wpa_cli select_network " + std::to_string(id);
		int32_t errorCode = system(command.c_str());
		if (errorCode != 0)
		{
			error("Failed to connect to network id=%d, code=%d", id, errorCode);
		}
	}

	static void save()
	{
		info("Saving wpa_supplicant configuration");
		int32_t errorCode = system("wpa_cli save_config");
		if (errorCode != 0)
		{
			error("Failed to save wpa_supplicant configuration, code=%d", errorCode);
		}
	}

	void enable(bool enable)
	{
		info("%s WiFi", enable ? "Enabling" : "Disabling");
		int32_t errorCode = system(enable ? "ip link set wlan0 up" : "ip link set wlan0 down");
		if (errorCode != 0)
		{
			error("Failed to %s WiFi, code=%d", enable ? "enable" : "disable", errorCode);
		}
	}

	void reconfigure()
	{
		info("Reconfiguring wpa_supplicant");
		int32_t errorCode = executeCommand("wpa_cli reconfigure");
		if (errorCode != 0)
		{
			error("Failed to reconfigure wpa_supplicant, code=%d", errorCode);
		}
	}

	std::vector<WiFiNetwork> getKnownWiFiNetworks()
	{
		info("Getting known WiFi networks");
		std::vector<WiFiNetwork> networks;
		// reconfigure();

		// sleep(1);
		std::string output = executeCommandWithOutput("wpa_cli -i " + INTERFACE + " list_networks");

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

			std::vector<std::string> fields;
			std::stringstream lineStream(line);
			std::string field;

			while (getline(lineStream, field, '\t'))
			{
				if (!field.empty())
				{
					fields.push_back(field);
				}
			}

			if (fields.size() >= 4)
			{
				std::string idStr = fields[0];
				std::string ssid = fields[1];
				std::string flags = fields[3];

				// Remove quotes from SSID if present
				if (ssid.size() >= 2 && ssid.front() == '"' && ssid.back() == '"')
				{
					ssid = ssid.substr(1, ssid.size() - 2);
				}

				WiFiNetwork network;
				network.ssid = ssid;
				network.id = stoi(idStr);
				network.connected = flags.find("[CURRENT]") != std::string::npos;
				networks.push_back(network);
				info("Found known network: \"%s\", id: %d, current: %d",
					 network.ssid.c_str(),
					 network.id,
					 network.connected);
			}
		}
		return networks;
	}

	std::vector<WiFiNetwork> scanWiFiNetworks()
	{
		std::vector<WiFiNetwork> knownNetworks = getKnownWiFiNetworks();
		info("Found %d known networks", knownNetworks.size());

		// sleep(2);
		info("Scanning for WiFi networks");
		std::vector<WiFiNetwork> networks;
		std::string output = executeCommandWithOutput("iw dev " + INTERFACE + " scan");

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
				network.ssid = line.substr(start);
				if (network.ssid.c_str()[0] != '\0')
				{
					network.id = -1;
					for (const WiFiNetwork& knownNetwork : knownNetworks)
					{
						if (network.ssid == knownNetwork.ssid)
						{
							info("Network: \"%s\" matches known network \"%s\"",
								 network.ssid.c_str(),
								 knownNetwork.ssid.c_str());
							network.id = knownNetwork.id;
							network.connected = knownNetwork.connected;
							break;
						}
					}
					info("Found network: \"%s\", signal: %d dBm, id: %d, %s",
						 network.ssid.c_str(),
						 network.signal_level,
						 network.id,
						 network.connected ? "connected" : "disconnected");
					networks.push_back(network);
					network.clear();
				}
			}
		}

		info("Sorting networks by signal level");
		std::sort(networks.begin(),
				  networks.end(),
				  [](const WiFiNetwork& a, const WiFiNetwork& b)
				  {
					  if (a.connected != b.connected)
					  {
						  return a.connected;
					  }
					  return a.signal_level > b.signal_level;
				  });
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
		info("Connecting to WiFi network \"%s\"", ssid.c_str());
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
		info("Connecting to WiFi network \"%s\"", ssid.c_str());
		if (!isNetworkKnown(ssid))
		{
			info("Network \"%s\" is not known, saving details", ssid.c_str());
			std::string command = "wpa_passphrase " + ssid + " " + password + " >>" + s_wpa_supplicant;
			int32_t errorCode = executeCommand(command);
			if (errorCode != 0)
			{
				error("Failed to save details for network \"%s\", code=%d", ssid.c_str(), errorCode);
				return;
			}
			reconfigure();
			sleep(2);
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
		info("Disconnecting from WiFi network");
		int32_t errorCode = system("wpa_cli disconnect");
	}

	void reconnect()
	{
		info("Reconnecting to WiFi network");
		int32_t errorCode = system("wpa_cli reconnect");
		if (errorCode != 0)
		{
			error("Failed to restart wpa_supplicant, code=%d", errorCode);
		}
	}

	void forgetNetwork(const std::string& ssid)
	{
		info("Forgetting network \"%s\"", ssid.c_str());
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
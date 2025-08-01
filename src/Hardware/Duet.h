/*
 * Duet.h
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_HARDWARE_DUET_H_
#define JNI_HARDWARE_DUET_H_

#include "Configuration.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/StringRef.h"
#include "termios.h"
#include "utils/utils.h"
#include <hv/requests.h>
#include <nlohmann/json.hpp>

namespace Comm
{
	constexpr const char* const duetCommunicationTypeNames[] = {"usb", "network", "uart"};

	typedef struct
	{
		unsigned int rate;
		unsigned int internal;
	} baudrate_t;

	constexpr baudrate_t baudRates[] = {{1200, B1200},
										{2400, B2400},
										{4800, B4800},
										{9600, B9600},
										{19200, B19200},
										{38400, B38400},
										{57600, B57600},
										{115200, B115200},
										{230400, B230400},
										{460800, B460800},
										{921600, B921600}};

	enum class CommunicationType
	{
		none = -1,
		usb,
		network,
		uart,
		COUNT
	};

	struct DuetConfig
	{
		std::string ipAddress = DEFAULT_IP_ADDRESS;
		std::string hostname = "";
		std::string password = "";
		std::chrono::milliseconds pollInterval = DEFAULT_PRINTER_POLL_INTERVAL;
		CommunicationType communicationType = CommunicationType::usb;
		unsigned int baudRate = B115200;

		// NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		// 	DuetConfig, ipAddress, hostname, password, pollInterval, communicationType, baudRate)
	};

	class Duet
	{
	  public:
		typedef int32_t error_code;

		static Duet& GetInstance()
		{
			static Duet instance;
			return instance;
		}

		void Init();
		void Reset();
		void Reconnect();

		void SetCommunicationType(CommunicationType type);
		const CommunicationType GetCommunicationType() const;
		const char* GetCommunicationTypeName() const;
		void SetPollInterval(std::chrono::milliseconds interval);
		void ScalePollIntervalScale(float scale);
		const std::chrono::milliseconds GetPollInterval() const;
		const std::chrono::milliseconds GetScaledPollInterval() const;
		const uint32_t GetNextLineNumber() { return m_nextLineNumber++; }

		void SendGcode(std::string_view gcode);
		void SendGcodef(const char* fmt, ...);
		void RequestReply(HttpResponse& r);
		void ProcessReply(HttpResponse& r);

		bool UploadFile(const char* filename, const std::string& contents);
		bool DownloadFile(std::string_view filename, std::string& contents);

		void RequestModel(const char* flags = "d99f");
		void RequestModel(const char* key, const char* flags);
		bool RequestFileList(const std::string& dir, const size_t first = 0);
		bool RequestFileInfo(const char* filename);
		bool RequestThumbnail(const char* filename, uint32_t offset);

		// UART methods
		void SetBaudRate(const unsigned int baudRateCode);
		void SetBaudRate(const baudrate_t& baudRate);
		const baudrate_t& GetBaudRate() const;

		// Network methods
		const bool Connect(bool useSessionKey = true);
		const bool Disconnect();
		const bool IsConnected() const { return m_connected; }
		const bool IsSbcMode() const { return m_sbcMode; }

		const std::string& GetBaseUrl() const;

		void SetIPAddress(const std::string& ipAddress);
		const std::string& GetIPAddress() const;
		void ClearIPAddress();

		void SetHostname(const std::string hostname);
		const std::string& GetHostname() const;

		void SetPassword(const std::string& password);
		const std::string& GetPassword() const;

		void SetSessionKey(const uint32_t sessionKey);

		// USB methods

	  private:
		Duet();

		void PrepareRequest(HttpRequest& req, const std::string& subUrl, hv::QueryParams& queryParameters);
		bool AsyncGet(const std::string& subUrl,
					  hv::QueryParams& queryParameters,
					  HttpResponseCallback callback,
					  bool queue);
		void AsyncGetInner(const HttpRequestPtr& req, HttpResponseCallback callback);
		bool AsyncGetCallback(const HttpRequestPtr& req, const HttpResponsePtr& r, HttpResponseCallback callback);
		bool Get(const std::string& subUrl, HttpResponse& r, hv::QueryParams& queryParameters);
		bool Post(const std::string& subUrl,
				  HttpResponse& r,
				  hv::QueryParams& queryParameters,
				  const std::string& data);
		void saveConfig();

		DuetConfig m_config;
		std::chrono::milliseconds m_lastRequestTime;
		float m_pollIntervalScale;
		uint32_t m_nextLineNumber = 0;
		std::mutex m_sendLock;

		// USB

		// Network
		hv::HttpClient m_cli; // for sendAsync() only!
		uint32_t m_sessionKey;
		std::chrono::milliseconds m_sessionTimeout;
		bool m_sbcMode;
		bool m_connected = false;

		static constexpr uint32_t sm_noSessionKey = 0;
	};

#define DUET Duet::GetInstance()
} // namespace Comm

#endif /* JNI_HARDWARE_DUET_H_ */

/*
 * Duet.cpp
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#include "Comm/Communication.h"
#include "Comm/FileInfo.h"
#include "Comm/JsonDecoder.h"
#include "Comm/Usb.h"
#include "Debug.h"

#include "Duet.h"

#include "Hardware/SerialIo.h"
#include "ObjectModel/PrinterStatus.h"
#include "ObjectModel/Utils.h"
#include "Storage.h"
#include "utils/StorageHelper.h"
#include "utils/TimeHelper.h"
#include "utils/utils.h"
#include <map>
#include <string>

namespace Comm
{
	void to_json(nlohmann::json& j, const DuetConfig& c)
	{
		j = nlohmann::json{
			{ID_DUET_HOSTNAME, c.hostname},
			{ID_DUET_PASSWORD, c.password},
			{ID_DUET_COMMUNICATION_TYPE, c.communicationType},
			{ID_DUET_POLL_INTERVAL, c.pollInterval},
			{ID_DUET_BAUD_RATE, c.baudRate},
		};
	}

	void from_json(const nlohmann::json& j, DuetConfig& c)
	{
		j.at(ID_DUET_HOSTNAME).get_to(c.hostname);
		j.at(ID_DUET_PASSWORD).get_to(c.password);
		j.at(ID_DUET_COMMUNICATION_TYPE).get_to(c.communicationType);
		j.at(ID_DUET_POLL_INTERVAL).get_to(c.pollInterval);
		j.at(ID_DUET_BAUD_RATE).get_to(c.baudRate);
	}

	Duet::Duet()
		: m_lastRequestTime(0)
		, m_pollIntervalScale(1.0f)
		, m_sessionKey(sm_noSessionKey)
		, m_sessionTimeout(0)
		, m_sbcMode(false)
	{
	}

	void Duet::Init()
	{
		// TODO restore from memory
		DuetConfig config = StorageHelper::getData<DuetConfig>(ID_DUET, DuetConfig());

		SetPollInterval((uint32_t)config.pollInterval);
		SetBaudRate(config.baudRate);
		SetIPAddress(config.ipAddress);
		SetHostname(config.hostname);
		SetPassword(config.password);
		SetCommunicationType((CommunicationType)config.communicationType);
	}

	void Duet::Reset()
	{
		verbose("Resetting Duet");
		m_sessionKey = sm_noSessionKey;
		m_sbcMode = false;
		m_sessionTimeout = 0;
		m_lastRequestTime = 0;
		m_pollIntervalScale = 1.0f;
		ClearIPAddress();

		OM::RemoveAll();
		Comm::ResetSeqs();
	}

	void Duet::Reconnect()
	{
		warn("");
		Disconnect();
		Connect();
	}

	void Duet::saveConfig()
	{
		StorageHelper::setData(ID_DUET, m_config);
	}

	void Duet::SetCommunicationType(CommunicationType type)
	{
		if (type == m_config.communicationType)
			return;
		info("Setting communication type to %d", (int)type);
		Disconnect();

		m_config.communicationType = type;
#if 0
		FILEINFO_CACHE->ClearCache();
#endif
		Connect();
		saveConfig();
	}

	const CommunicationType Duet::GetCommunicationType() const
	{
		return m_config.communicationType;
	}

	void Duet::SetPollInterval(uint32_t interval)
	{
		if (interval < MIN_PRINTER_POLL_INTERVAL)
		{
			warn("Poll interval too low, setting to %d", MIN_PRINTER_POLL_INTERVAL);
			interval = MIN_PRINTER_POLL_INTERVAL;
		}
		info("Setting poll interval to %u (scaled to %u)",
			 interval,
			 static_cast<uint32_t>(interval * m_pollIntervalScale));

		m_config.pollInterval = interval;
		saveConfig();
		// resetUserTimer(TIMER_UPDATE_DATA, static_cast<int>(m_pollInterval * m_pollIntervalScale));
	}

	void Duet::ScalePollIntervalScale(float scale)
	{
		if (scale <= 0.0f)
		{
			warn("Invalid scale factor %f", scale);
			return;
		}

		info("Scalling poll interval by %f from %u to %u",
			 scale,
			 GetScaledPollInterval(),
			 static_cast<uint32_t>(m_config.pollInterval * scale));

		m_pollIntervalScale = scale;
		saveConfig();
		// resetUserTimer(TIMER_UPDATE_DATA, static_cast<int>(m_pollInterval * m_pollIntervalScale));
	}

	const uint32_t Duet::GetPollInterval() const
	{
		return m_config.pollInterval;
	}

	const uint32_t Duet::GetScaledPollInterval() const
	{
		return static_cast<uint32_t>(m_config.pollInterval * m_pollIntervalScale);
	}

	void Duet::PrepareRequest(HttpRequest& req, const char* subUrl, hv::QueryParams& queryParameters)
	{
		req.method = HTTP_GET;
		req.host = GetBaseUrl();
		req.path = subUrl;
		req.headers["Connection"] = "keep-alive";
		req.headers["Accept"] = "application/json";
		req.headers["Content-Type"] = "application/json";
		if (m_sessionKey != sm_noSessionKey)
		{
			// TODO: Determine why session key isn't working
			// req.headers["X-Session-Key"] = utils::format("%u", m_sessionKey).c_str();
		}
		req.query_params = queryParameters;
		req.timeout = HTTP_TIMEOUT;

		req.DumpUrl();
	}

	bool Duet::AsyncGet(const char* path,
						hv::QueryParams& queryParameters,
						HttpResponseCallback callback,
						bool queue = false)
	{
#if 1
		if (((!m_sbcMode && m_sessionKey == sm_noSessionKey) ||
			 (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout)) &&
			(strncmp(path, "/rr_connect", 11) != 0))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send get request %s", path);
				return false;
			}
		}

		auto req = std::make_shared<HttpRequest>();
		PrepareRequest(*req, path, queryParameters);
		dbg("Get (async): \"%s\", sessionKey=%u", req->url.c_str(), m_sessionKey);

		// `sendAsync()` requires the client to still be alive later and does appear to be thread safe using a single
		// client
		m_cli.sendAsync(req,
						[req, callback](const HttpResponsePtr& resp)
						{
							if (resp == NULL)
							{
								error("request \"%s\" failed!", req->url.c_str());
							}
							else
							{
								dbg("Response (async): %s %s", req->url.c_str(), resp->status_message());
								verbose("%s", resp->body.c_str());
								callback(resp);
							}
						});
		m_lastRequestTime = TimeHelper::getCurrentTime();
#endif
		return true;
	}

	/*
	Tries to make a get request to Duet, if it returns 401 or 403 then it will run `rr_connect` and send the request
	again
	*/
	bool Duet::Get(const char* path, HttpResponse& r, hv::QueryParams& queryParameters)
	{
		if (((!m_sbcMode && m_sessionKey == sm_noSessionKey) ||
			 (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout)) &&
			(strncmp(path, "/rr_connect", 11) != 0))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send get request %s", path);
				r.status_code = HTTP_STATUS_NOT_FOUND;
				return false;
			}
		}

		HttpRequest req;
		PrepareRequest(req, path, queryParameters);
		dbg("\"%s\", sessionKey=%u", req.url.c_str(), m_sessionKey);

		hv::HttpClient cli;
		cli.send(&req, &r); // `send()` is not thread safe if using the same client so client is created on stack

		dbg("Response (async): %s %s", req.url.c_str(), r.status_message());

		if (r.status_code != HTTP_STATUS_OK)
		{
			error("HTTP error %d: Likely invalid sessionKey %u.", r.status_code, m_sessionKey);
			return false;
		}
		verbose("%s", r.body.c_str());
		m_lastRequestTime = TimeHelper::getCurrentTime();
		return true;
	}

	/*
	Tries to make a post request to Duet, if it returns 401 or 403 then it will run `rr_connect` and send the request
	again
	*/
	bool Duet::Post(const char* subUrl, HttpResponse& r, hv::QueryParams& queryParameters, const std::string& data)
	{
#if 0
		if ((!m_sbcMode && m_sessionKey == sm_noSessionKey) ||
			(TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send post request %s", subUrl);
				return false;
			}
		}
		if (!Comm::Post(GetBaseUrl(), subUrl, r, queryParameters, data, m_sessionKey))
		{
			if (r.code == 401 || r.code == 403)
			{
				error("HTTP error %d: Likely invalid sessionKey %d. Running rr_connect", r.code, m_sessionKey);
				Connect();
				return Comm::Post(GetBaseUrl(), subUrl, r, queryParameters, data, m_sessionKey);
			}
			return false;
		}
		m_lastRequestTime = TimeHelper::getCurrentTime();
#endif
		return true;
	}

	void Duet::SendGcode(const std::string& gcode)
	{
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
			SerialIo::Send(gcode);
			break;
		case CommunicationType::network:
		{
			HttpResponse r;
			hv::QueryParams query;
			query["gcode"] = gcode;
			AsyncGet(
				"/rr_gcode",
				query,
				[this, gcode](const HttpResponsePtr& r)
				{
					if (r->status_code != HTTP_STATUS_OK)
					{
						printf("HTTP error %d: Failed to send gcode: %s", r->status_code, gcode.c_str());
						return false;
					}
					return true;
				},
				true);
			break;
		}
		case CommunicationType::usb:
		{
			UsbDevice& usb = getCurrentUsbDevice();
			if (!usb.isConnected())
			{
				warn("USB device not connected");
				connectUsbDevice();
			}
			usb.send(gcode.c_str());
			break;
		}
		default:
			break;
		}
	}

	void Duet::SendGcodef(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		SendGcode(utils::vformat(fmt, args).c_str());
		va_end(args);
	}

	bool Duet::UploadFile(const char* filename, const std::string& contents)
	{
		info("Uploading file %s: %d bytes", filename, contents.size());
		// TODO add sleep

		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		{
			/* UART is too slow to support uploading files */
			if (contents.size() > MAX_UART_UPLOAD_SIZE)
			{
				warn("File too large (%u) to upload via UART, limit is %u", contents.size(), MAX_UART_UPLOAD_SIZE);
				return false;
			}

			SendGcodef("M28 \"%s\"", filename);
			size_t prevPosition = 0;
			size_t position = contents.find("\n"); // Find the first occurrence of \n
			std::string line;
			while (position != std::string::npos)
			{
				line = contents.substr(prevPosition, position - prevPosition);
				prevPosition = position + 1;
				position = contents.find("\n", position + 1); // Find the next occurrence, if any
				SendGcode(line.c_str());
			}
			SendGcode("M29");
			break;
		}
		case CommunicationType::network:
		{
			// TODO network upload
#if 0
			registerDelayedCallback("upload_file_progress", 1000, []() {
				static int s_progress = 0;
				if (s_progress >= 90)
					return false;
				s_progress += 10;
				return true;
			});
			HttpResponse r;
			hv::QueryParams query;
			query["name"] = filename;
			if (!Post("/rr_upload", r, query, contents))
			{
				printf(utils::format("HTTP error %d %s: Failed to upload file: %s", r.code, r.body, filename).c_str());
				unregisterDelayedCallback("upload_file_progress");
				return false;
			}
#endif
			break;
		}
		default:
			break;
		}
		return true;
	}

	bool Duet::DownloadFile(const char* filename, std::string& contents)
	{
		info("Downloading file %s", filename);
		switch (m_config.communicationType)
		{
		case CommunicationType::network:
		{
#if 0
			HttpResponse r;
			hv::QueryParams query;
			query["name"] = filename;
			if (!Get("/rr_download", r, query))
			{
				printf("HTTP error %d: Failed to download file: %s", r.code, filename);
				return false;
			}
			contents = r.body;
#endif
			break;
		}
		default:
			warn("Communication type not supported for downloading files");
			return false;
		}
		return true;
	}

	void Duet::RequestModel(const char* flags)
	{
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
			SendGcodef("M409 F\"%s\"\n", flags);
			break;
		case CommunicationType::network:
		{
			hv::QueryParams query;
			query["flags"] = flags;
#if ASYNC_RR_MODEL
			AsyncGet("/rr_model",
					 query,
					 [this, flags](const HttpResponsePtr& r)
					 {
						 JsonDecoder decoder;
						 if (r->status_code != HTTP_STATUS_OK)
						 {
							 error("HTTP error %d: Failed to get model update for flags: %s", r->status_code, flags);
							 return false;
						 }
						 decoder.CheckInput((const unsigned char*)r->body.c_str(), r->body.length() + 1);
						 return true;
					 });
#else
			HttpResponse r;
			Get("/rr_model", r, query);
			JsonDecoder decoder;
			if (r.status_code != HTTP_STATUS_OK)
			{
				error("HTTP error %d: Failed to get model update for flags: %s", r.status_code, flags);
				break;
			}
			decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
#endif
			break;
		}
		default:
			break;
		}
	}

	void Duet::RequestModel(const char* key, const char* flags)
	{
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
			SendGcodef("M409 K\"%s\" F\"%s\"\n", key, flags);
			break;
		case CommunicationType::network:
		{
			hv::QueryParams query;
			query["key"] = key;
			query["flags"] = flags;
#if ASYNC_RR_MODEL
			AsyncGet("/rr_model",
					 query,
					 [this, key, flags](const HttpResponsePtr& r)
					 {
						 JsonDecoder decoder;
						 if (r->status_code != HTTP_STATUS_OK)
						 {
							 error("HTTP error %d: Failed to get model update for key: %s, flags: %s",
								   r->status_code,
								   key,
								   flags);
							 return false;
						 }
						 decoder.CheckInput((const unsigned char*)r->body.c_str(), r->body.length() + 1);
						 return true;
					 });
#else
			HttpResponse r;
			Get("/rr_model", r, query);
			JsonDecoder decoder;
			if (r.status_code != HTTP_STATUS_OK)
			{
				error("HTTP error %d: Failed to get model update for key: %s, flags: %s", r.status_code, key, flags);
				break;
			}
			decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
#endif
			break;
		}
		default:
			break;
		}
		return;
	}

	void Duet::RequestFileList(const char* dir, const size_t first)
	{
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
			SendGcodef("M20 S3 P\"%s\" R%d\n", dir, first);
			break;
		case CommunicationType::network:
		{
			JsonDecoder decoder;
			HttpResponse r;
			hv::QueryParams query;
			query["dir"] = dir;
			query["first"] = utils::format("%d", first);
			if (!Get("/rr_filelist", r, query))
			{
				error("HTTP error %d (%s): Failed to get file list for %s", r.status_code, r.status_message(), dir);
				break;
			}
			decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
			break;
		}
		default:
			warn("Communication type not supported for requesting file list");
			break;
		}
		return;
	}

	void Duet::RequestFileInfo(const char* filename)
	{
		dbg("for %s", filename);
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M36 \"%s\"", filename);
			break;
		case CommunicationType::network:
		{
			JsonDecoder decoder;
			hv::QueryParams query;
			query["name"] = filename;

#if 0
			AsyncGet(
				"/rr_fileinfo",
				query,
				[this](HttpResponse& r) -> bool
				{
					JsonDecoder decoder;
					if (r.code != 200)
					{
						return false;
					}
					decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
					return true;
				},
				true);

			break;
#endif

/* This way is quicker but duplicates code to decode the received data */
#if 0
			std::string name(filename);
			AsyncGet(
				"/rr_fileinfo",
				query,
				[this, name](HttpResponse& r) -> bool {
					Json::Reader reader;
					Json::Value body;
					const char* filename = name.c_str();
					dbg("Name = %s", filename);
					if (r.code != 200)
					{
						printf(
							utils::format("HTTP error %d: Failed to get file info for file: %s", r.code, r.body)
								.c_str());
						return false;
					}
					reader.parse(r.body, body);
					if (body.isMember("err") && body["err"].asInt() != 0)
					{
						printf(
							utils::format(
								"Failed to get file info for file: %s, returned error %d", r.body, body["err"].asInt())
								.c_str());
						return false;
					}
					if (!body.isMember("thumbnails"))
					{
						info("No thumbnails found for %s", filename);
						return false;
					}
					Json::Value thumbnailsJson = body["thumbnails"];
					for (Json::ArrayIndex i = 0; i < thumbnailsJson.size(); i++)
					{
						Thumbnail thumbnail;
						ThumbnailContext context;
						ThumbnailInit(thumbnail);
						if (!thumbnailsJson[i].isMember("width"))
						{
							continue;
						}
						thumbnail.meta.width = thumbnailsJson[i]["width"].asInt();

						if (!thumbnailsJson[i].isMember("height"))
						{
							continue;
						}
						thumbnail.meta.height = thumbnailsJson[i]["height"].asInt();

						if (!thumbnailsJson[i].isMember("offset"))
						{
							continue;
						}
						context.next = thumbnailsJson[i]["offset"].asInt();

						if (!thumbnailsJson[i].isMember("format"))
						{
							continue;
						}
						std::string format = thumbnailsJson[i]["format"].asString();
						if (!thumbnail.meta.SetImageFormat(format.c_str()))
						{
							warn("Unsupported thumbnail format: %s", format.c_str());
							continue;
						}
						thumbnail.image.New(thumbnail.meta, filename);

						info("File %s has thumbnail %d: %dx%d", filename, i, thumbnail.width, thumbnail.height);

						hv::QueryParams query;
						query["name"] = filename;
						while (context.next != 0)
						{
							// Request thumbnail data
							query["offset"] = utils::format("%d", context.next);
							info("Requesting thumbnail data for %s at offset %d\n", filename, context.next);
							if (!Get("/rr_thumbnail", r, query))
							{
								error("Failed to get thumbnail data for %s at offset %d", filename, context.next);
								continue;
							}
							dbg("Parsing rr_thumbnail response");
							reader.parse(r.body, body);

							if (body.isMember("err") && body["err"].asInt() != 0)
							{
								error("Failed to get thumbnail data for %s at offset %d: %d",
									  filename,
									  context.next,
									  body["err"].asInt());
								continue;
							}

							if (body.isMember("next"))
							{
								context.next = body["next"].asInt();
								dbg("Next thumbnail offset: %d", context.next);
							}

							dbg("Decoding thumbnail data");
							if (body.isMember("data"))
							{
								ThumbnailBuf data;
								data.size = std::min(body["data"].asString().size(), sizeof(data.buffer));
								memcpy(data.buffer, body["data"].asString().c_str(), data.size);
								ThumbnailDecodeChunk(thumbnail, data);
							}
						}
						thumbnail.Close();
						OM::FileSystem::GetListView()->refreshListView();
					}
					return true;
				},
				true);
			break;
#endif
		}
		default:
			break;
		}
		return;
	}

	void Duet::RequestThumbnail(const char* filename, uint32_t offset)
	{
		dbg("for %s, offset=%u", filename, offset);
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M36.1 P\"%s\" S%d", filename, offset);
			break;
		case CommunicationType::network:
		{
#if 0
			hv::QueryParams query;
			query["name"] = filename;
			query["offset"] = utils::format("%d", offset);
			AsyncGet(
				"/rr_thumbnail",
				query,
				[this](HttpResponse& r) -> bool
				{
					JsonDecoder decoder;
					if (r.code != 200)
					{
						return false;
					}
					decoder.SetPrefix("thumbnail:");
					decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.size() + 1);
					return true;
				},
				true);
#endif
			break;
		}
		default:
			break;
		}
	}

	void Duet::ProcessReply(HttpResponse& reply)
	{
		if (m_config.communicationType != CommunicationType::network)
			return;

		if (reply.body.empty())
		{
			warn("Empty reply received");
			return;
		}

		JsonDecoder decoder;
		if (reply.body[0] != '{')
		{
			dbg("Reply not json: assuming it is a gcode response");

			dbg("Removing \\r from reply body");
			utils::removeCharFromString(reply.body, '\r');
			size_t prevPosition = 0;
			size_t position = reply.body.find("\n"); // Find the first occurrence of \n

			// Split reply by new line and handle each as its own response.
			// The replicates the uart behaviour and is required because rr_reply will group multiple
			// replies together into a single response.
			StringRef ref((char*)"resp", 5);
			size_t indices[MAX_ARRAY_NESTING] = {0};
			while (position != std::string::npos)
			{
				std::string line = reply.body.substr(prevPosition, position - prevPosition);
				dbg("line: %s", line.c_str());
				prevPosition = position + 1;
				position = reply.body.find("\n", position + 1); // Find the next occurrence, if any
				verbose("position=%u, prevPosition=%u", position, prevPosition);
				if (line.empty())
				{
					verbose("Skipping empty line");
					continue;
				}
				// Can skip checking the input since we know it's a gcode response
				decoder.ProcessReceivedValue(ref, line.c_str(), indices);
			}
			return;
		}
		decoder.CheckInput((const unsigned char*)reply.body.c_str(), reply.body.length() + 1);
		return;
	}

	void Duet::RequestReply(HttpResponse& r)
	{
		hv::QueryParams query;
		Get("/rr_reply", r, query);
	}

	const bool Duet::Connect(bool useSessionKey)
	{
		Disconnect();
		Reset();

		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		{
			// TODO open UART connection
			info("Opening UART %s at %u", DEFAULT_UART_PORT, GetBaudRate().rate);
			SerialIo::Init(DEFAULT_UART_PORT, GetBaudRate().internal);
			return true;
		}
		case CommunicationType::network:
		{
			info("Connecting to Duet at %s", GetBaseUrl().c_str());

			HttpResponse r;
			hv::QueryParams query;
			query["password"] = std::string("\"") + m_config.password + "\"";
			if (useSessionKey)
				query["sessionKey"] = "yes";

			Get("/rr_connect", r, query);

			if (r.status_code != HTTP_STATUS_OK)
			{
				error("rr_connect failed, returned response %d", r.status_code);
				return false;
			}

			verbose("parsing rr_connect response");
			auto body = nlohmann::json::parse(r.body, nullptr, false);
			if (body.is_discarded())
			{
				error("Failed to parse JSON response from rr_connect");
				return false;
			}

			if (body.contains("err") && body["err"].get<int>() != 0)
			{
				error("rr_connect failed, returned error %d", body["err"].get<int>());
				return false;
			}

			if (body.contains("sessionTimeout"))
			{
				m_sessionTimeout = body["sessionTimeout"].get<int>();
				m_lastRequestTime = TimeHelper::getCurrentTime();
				info("Duet session timeout set to %d", m_sessionTimeout);
			}

			if (body.contains("sessionKey"))
			{
				SetSessionKey(body["sessionKey"].get<unsigned int>());
				info("Duet session key = %u", m_sessionKey);
			}
			if (body.contains("isEmulated"))
			{
				SetSessionKey(sm_noSessionKey);
				m_sbcMode = true;
				info("Connected to Duet in SBC mode");
			}
			info("rr_connect succeeded");
			return true;
		}
		case CommunicationType::usb:
		{
			connectUsbDevice();
			break;
		}
		default:
			break;
		}

		return false;
	}

	const Duet::error_code Duet::Disconnect()
	{
		SetStatus(OM::PrinterStatus::connecting);
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
			SerialIo::Shutdown();
			break;
		case CommunicationType::network:
		{
#if 0
			ClearThreadPool();
			if (m_sessionKey == sm_noSessionKey)
			{
				Reset();
				return 0;
			}
			HttpResponse r;
			hv::QueryParams query;
			if (!Comm::Get(GetBaseUrl(), "/rr_disconnect", r, query, m_sessionKey))
			{
				error("rr_disconnect failed, returned response %d", r.code);
				return r.code;
			}
			Reset();
			return r.code;
#else
			return 200;
#endif
		}
		case CommunicationType::usb:
		{
			getCurrentUsbDevice().reset();
			break;
		}
		default:
			break;
		}
		return 0;
	}

	const std::string& Duet::GetBaseUrl() const
	{
		if (!m_config.ipAddress.empty())
		{
			verbose("Using IP address %s", m_config.ipAddress.c_str());
			return m_config.ipAddress;
		}
		verbose("Using hostname %s", m_config.hostname.c_str());
		return m_config.hostname;
	}

	void Duet::SetBaudRate(const unsigned int baudRateCode)
	{
		for (unsigned int i = 0; i < ARRAY_SIZE(baudRates); i++)
		{
			if (baudRates[i].internal == baudRateCode)
			{
				SetBaudRate(baudRates[i]);
				return;
			}
		}
		warn("Baud rate %u not found", baudRateCode);
	}

	void Duet::SetBaudRate(const baudrate_t& baudRate)
	{
		info("Setting baud rate to %u (%u)", baudRate.rate, baudRate.internal);
		SerialIo::SetBaudRate(baudRate.internal);
		m_config.baudRate = baudRate.internal;
		saveConfig();
	}

	const baudrate_t& Duet::GetBaudRate() const
	{
		for (auto& baud : baudRates)
		{
			if (baud.internal == m_config.baudRate)
			{
				return baud;
			}
		}
		warn("Baud rate %u not found", m_config.baudRate);
		return baudRates[0];
	}

	void Duet::SetIPAddress(const std::string& ipAddress)
	{
		m_config.ipAddress = ipAddress;
		saveConfig();
	}

	const std::string& Duet::GetIPAddress() const
	{
		return m_config.ipAddress;
	}

	void Duet::ClearIPAddress()
	{
		m_config.ipAddress.clear();
		dbg("IP address cleared \"%s\"", m_config.ipAddress.c_str());
	}

	void Duet::SetHostname(const std::string hostname)
	{
		dbg("Hostname = %s", hostname.c_str());
		// TODO store hostname
		m_config.hostname.clear();

		if (hostname.find("http://") == 0)
		{
			m_config.hostname = hostname.substr(7);
		}
		else if (hostname.find("https://") == 0)
		{
			m_config.hostname = hostname.substr(8);
		}
		else
		{
			m_config.hostname = hostname;
		}

		ClearIPAddress();
		info("Set Duet hostname to %s", m_config.hostname.c_str());
		// TODO Clear file info cache
		// FILEINFO_CACHE->ClearCache();
		if (m_config.communicationType == CommunicationType::network)
			Connect();

		saveConfig();
	}

	const std::string& Duet::GetHostname() const
	{
		return m_config.hostname;
	}

	void Duet::SetPassword(const std::string& password)
	{
		m_config.password = password;
		saveConfig();
	}

	const std::string& Duet::GetPassword() const
	{
		return m_config.password;
	}

	void Duet::SetSessionKey(const uint32_t key)
	{
		m_sessionKey = key;
		info("Set Duet session key = %u", m_sessionKey);
	}
} // namespace Comm

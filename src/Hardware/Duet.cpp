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

#include "Duet3D/General/CRC16.h"
#include "Hardware/SerialIo.h"
#include "ObjectModel/PrinterStatus.h"
#include "ObjectModel/Utils.h"
#include "Storage.h"
#include "UI/Core/Model.h"
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
		LOG_VERBOSE("Resetting Duet");
		m_sessionKey = sm_noSessionKey;
		m_sbcMode = false;
		m_sessionTimeout = 0;
		m_lastRequestTime = 0;
		m_pollIntervalScale = 1.0f;
		m_nextLineNumber = 0;
		ClearIPAddress();

		OM::RemoveAll();
		Comm::ResetSeqs();
	}

	void Duet::Reconnect()
	{
		LOG_INFO("Reconnecting...");
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
		LOG_INFO("Setting communication type to {:d}", (int)type);
		Disconnect();

		m_config.communicationType = type;
#if 1
		FILEINFO_CACHE->ClearCache();
#endif
		Connect();
		saveConfig();
	}

	const CommunicationType Duet::GetCommunicationType() const
	{
		return m_config.communicationType;
	}

	const char* Duet::GetCommunicationTypeName() const
	{
		CommunicationType type = m_config.communicationType;
		if (type >= CommunicationType::COUNT || type <= CommunicationType::none)
		{
			return "unknown";
		}
		return duetCommunicationTypeNames[(int)type];
	}

	void Duet::SetPollInterval(uint32_t interval)
	{
		if (interval < MIN_PRINTER_POLL_INTERVAL)
		{
			LOG_WARN("Poll interval too low, setting to {:d}", MIN_PRINTER_POLL_INTERVAL);
			interval = MIN_PRINTER_POLL_INTERVAL;
		}
		LOG_INFO("Setting poll interval to {:d} (scaled to {:d})",
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
			LOG_WARN("Invalid scale factor {:g}", scale);
			return;
		}

		LOG_INFO("Scalling poll interval by {:g} from {:d} to {:d}",
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

	void Duet::PrepareRequest(HttpRequest& req, const std::string& subUrl, hv::QueryParams& queryParameters)
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

	void Duet::AsyncGetInner(const HttpRequestPtr& req, HttpResponseCallback callback)
	{
		m_cli.sendAsync(req,
						[this, req, callback](const HttpResponsePtr& resp) { AsyncGetCallback(req, resp, callback); });
		m_lastRequestTime = TimeHelper::getCurrentTime();
	}

	bool Duet::AsyncGetCallback(const HttpRequestPtr& req, const HttpResponsePtr& r, HttpResponseCallback callback)
	{
		if (r == NULL)
		{
			LOG_ERROR("request \"{:s}\" failed!", req->url.c_str());
			// AsyncGetInner(req, callback);
			return false;
		}
		LOG_DBG("Response (async): {:s} {:s}", req->url.c_str(), r->status_message());
		LOG_VERBOSE("{:s}", r->body.c_str());
		callback(r);
		return true;
	}

	bool Duet::AsyncGet(const std::string& path,
						hv::QueryParams& queryParameters,
						HttpResponseCallback callback,
						bool queue = false)
	{
#if 1
		if (!IsConnected() && path != "/rr_connect")
		{
			LOG_DBG("Not connected to Duet, cannot send get request {:s}", path);
			return false;
		}

		if (((!m_sbcMode && m_sessionKey == sm_noSessionKey) ||
			 (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout)) &&
			(path != "/rr_connect"))
		{
			if (!Connect())
			{
				LOG_WARN("Failed to connect to Duet, cannot send get request {:s}", path);
				return false;
			}
		}

		auto req = std::make_shared<HttpRequest>();
		PrepareRequest(*req, path, queryParameters);
		LOG_DBG("Get (async): \"{:s}\", sessionKey={:d}", req->url.c_str(), m_sessionKey);

		// `sendAsync()` requires the client to still be alive later and does appear to be thread safe using a single
		// client

		AsyncGetInner(req, callback);
#endif
		return true;
	}

	/*
	Tries to make a get request to Duet, if it returns 401 or 403 then it will run `rr_connect` and send the request
	again
	*/
	bool Duet::Get(const std::string& path, HttpResponse& r, hv::QueryParams& queryParameters)
	{
		if (!IsConnected() && path != "/rr_connect")
		{
			LOG_DBG("Not connected to Duet, cannot send get request {:s}", path);
			return false;
		}

		if (((!m_sbcMode && m_sessionKey == sm_noSessionKey) ||
			 (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout)) &&
			(path != "/rr_connect"))
		{
			if (!Connect())
			{
				LOG_WARN("Failed to connect to Duet, cannot send get request {:s}", path);
				r.status_code = HTTP_STATUS_NOT_FOUND;
				return false;
			}
		}

		HttpRequest req;
		PrepareRequest(req, path, queryParameters);
		LOG_DBG("\"{:s}\", sessionKey={:d}", req.url.c_str(), m_sessionKey);

		hv::HttpClient cli;
		cli.send(&req, &r); // `send()` is not thread safe if using the same client so client is created on stack

		LOG_DBG("Response (async): {:s} {:s}", req.url.c_str(), r.status_message());

		if (r.status_code != HTTP_STATUS_OK)
		{
			LOG_ERROR("HTTP error {:d}: Likely invalid sessionKey {:d}.", (int)r.status_code, m_sessionKey);
			return false;
		}
		LOG_VERBOSE("{:s}", r.body.c_str());
		m_lastRequestTime = TimeHelper::getCurrentTime();
		return true;
	}

	/*
	Tries to make a post request to Duet, if it returns 401 or 403 then it will run `rr_connect` and send the request
	again
	*/
	bool Duet::Post(const std::string& subUrl,
					HttpResponse& r,
					hv::QueryParams& queryParameters,
					const std::string& data)
	{
#if 0
		if ((!m_sbcMode && m_sessionKey == sm_noSessionKey) ||
			(TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				LOG_WARN("Failed to connect to Duet, cannot send post request {:s}", subUrl);
				return false;
			}
		}
		if (!Comm::Post(GetBaseUrl(), subUrl, r, queryParameters, data, m_sessionKey))
		{
			if (r.code == 401 || r.code == 403)
			{
				LOG_ERROR("HTTP error {:d}: Likely invalid sessionKey {:d}. Running rr_connect", (int)r.code, m_sessionKey);
				Connect();
				return Comm::Post(GetBaseUrl(), subUrl, r, queryParameters, data, m_sessionKey);
			}
			return false;
		}
		m_lastRequestTime = TimeHelper::getCurrentTime();
#endif
		return true;
	}

	void Duet::SendGcode(std::string_view gcode)
	{
		if (!IsConnected())
		{
			LOG_DBG("Not connected to Duet, cannot send gcode: {:s}", gcode);
			return;
		}
		LOG_DBG("Sending gcode: '{:s}'", gcode);

		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
		{
			std::lock_guard<std::mutex> lock(m_sendLock);
			CRC16 crc;
			size_t len = 0;
			std::string_view line;
			auto send_cb = m_config.communicationType == CommunicationType::uart ? SerialIo::Send : sendUsbData;

			for (size_t i = 0; i < gcode.length(); i++)
			{
				char c = gcode[i];
				if (c == '\n')
				{
					line = gcode.substr(i - len, len);
					send_cb(line);
					send_cb(fmt::format("*{:05d}\n", crc.Get()));
					len = 0;
					crc.Reset(0);
					continue;
				}
				if (len == 0)
				{
					uint32_t lineNumber = GetNextLineNumber();
					std::string lineNumberStr = fmt::format("N{:d} ", lineNumber);
					for (char c : lineNumberStr)
					{
						crc.Update(c);
					}
					send_cb(lineNumberStr);
				}
				len++;
				crc.Update(c);
			}
			if (len > 0)
			{
				line = gcode.substr(gcode.length() - len, len);
				send_cb(line);
				send_cb(fmt::format("*{:05d}\n", crc.Get()));
			}
			break;
		}
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
						LOG_ERROR("HTTP error {:d}: Failed to send gcode: {:s}", (int)r->status_code, gcode);
						return false;
					}
					return true;
				},
				true);
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
		if (!IsConnected())
		{
			LOG_DBG("Not connected to Duet, cannot upload file {:s}", filename);
			return false;
		}

		LOG_INFO("Uploading file {:s}: {:d} bytes", filename, contents.size());
		// TODO add sleep

		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
		{
			/* UART is too slow to support uploading files */
			if (m_config.communicationType == CommunicationType::uart && contents.size() > MAX_UART_UPLOAD_SIZE)
			{
				LOG_WARN(
					"File too large ({:d}) to upload via UART, limit is {:d}", contents.size(), MAX_UART_UPLOAD_SIZE);
				return false;
			}

			SendGcodef("M28 \"%s\"\n", filename);
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
			SendGcode("M29\n");
			break;
		}
		case CommunicationType::network:
		{
			HttpResponse r;
			hv::QueryParams query;
			query["name"] = filename;
			if (!Post("/rr_upload", r, query, contents))
			{
				LOG_ERROR("HTTP error {:d} {:s}: Failed to upload file: {:s}", (int)r.status_code, r.body, filename);
				return false;
			}
			break;
		}
		default:
			break;
		}
		return true;
	}

	bool Duet::DownloadFile(std::string_view filename, std::string& contents)
	{
		if (!IsConnected())
		{
			LOG_DBG("Not connected to Duet, cannot download file {:s}", filename);
			return false;
		}

		LOG_INFO("Downloading file {}", filename);
		switch (m_config.communicationType)
		{
		case CommunicationType::network:
		{
#if 1
			HttpResponse r;
			hv::QueryParams query;
			query["name"] = filename;
			if (!Get("/rr_download", r, query))
			{
				LOG_ERROR("HTTP error {:d}: Failed to download file: {}", (int)r.status_code, filename);
				return false;
			}
			contents = r.body;
#endif
			break;
		}
		default:
			LOG_WARN("Communication type not supported for downloading files");
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
							 LOG_ERROR("HTTP error {:d}: Failed to get model update for flags: {:s}",
									   (int)r->status_code,
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
				LOG_ERROR("HTTP error {:d}: Failed to get model update for flags: {:s}", (int)r.status_code, flags);
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
							 LOG_ERROR("HTTP error {:d}: Failed to get model update for key: {:s}, flags: {:s}",
									   (int)r->status_code,
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
				LOG_ERROR("HTTP error {:d}: Failed to get model update for key: {:s}, flags: {:s}",
						  (int)r.status_code,
						  key,
						  flags);
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

	bool Duet::RequestFileList(const std::string& dir, const size_t first)
	{
		bool ret = true;
		LOG_DBG("dir = {:s}, first = {:d}", dir, first);
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
			SendGcodef("M20 S3 P\"%s\" R%d\n", dir.c_str(), first);
			break;
		case CommunicationType::network:
		{
			JsonDecoder decoder;
			HttpResponse r;
			hv::QueryParams query;
			query["dir"] = dir;
			query["first"] = utils::format("%d", first);
			ret = AsyncGet(
				"/rr_filelist",
				query,
				[this, dir](const HttpResponsePtr& r) -> bool
				{
					JsonDecoder decoder;
					if (r->status_code != 200)
					{
						LOG_ERROR("HTTP error {:d} ({:s}): Failed to get file list for {:s}",
								  (int)r->status_code,
								  r->status_message(),
								  dir);
						return false;
					}
					decoder.CheckInput((const unsigned char*)r->body.c_str(), r->body.length() + 1);
					return true;
				},
				true);
			break;
		}
		default:
			LOG_WARN("Communication type not supported for requesting file list");
			break;
		}
		return ret;
	}

	bool Duet::RequestFileInfo(const char* filename)
	{
		LOG_DBG("for {:s}", filename);
		bool ret = true;
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
			SendGcodef("M36 \"%s\"\n", filename);
			break;
		case CommunicationType::network:
		{
			JsonDecoder decoder;
			hv::QueryParams query;
			query["name"] = filename;

#if 1
			ret = AsyncGet(
				"/rr_fileinfo",
				query,
				[this](const HttpResponsePtr& r) -> bool
				{
					JsonDecoder decoder;
					if (r->status_code != 200)
					{
						LOG_ERROR("HTTP error {:d}: Failed to get file info for file: {:s}",
								  (int)r->status_code,
								  r->body.c_str());
						return false;
					}
					decoder.CheckInput((const unsigned char*)r->body.c_str(), r->body.length() + 1);
					return true;
				},
				true);

			break;
#endif

/* This way is quicker but duplicates code to decode the received data */
#if 0
			std::string name(filename);
			ret = AsyncGet(
				"/rr_fileinfo",
				query,
				[this, name](HttpResponse& r) -> bool {
					Json::Reader reader;
					Json::Value body;
					const char* filename = name.c_str();
					LOG_DBG("Name = {:s}", filename);
					if (r.code != 200)
					{
						LOG_ERROR("HTTP error {:d}: Failed to get file info for file: {:s}", (int)r.code, r.body);
						return false;
					}
					reader.parse(r.body, body);
					if (body.isMember("err") && body["err"].asInt() != 0)
					{
						LOG_ERROR("Failed to get file info for file: {:s}, returned error {:d}", r.body, body["err"].asInt());
						return false;
					}
					if (!body.isMember("thumbnails"))
					{
						LOG_INFO("No thumbnails found for {:s}", filename);
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
							LOG_WARN("Unsupported thumbnail format: {:s}", format.c_str());
							continue;
						}
						thumbnail.image.New(thumbnail.meta, filename);

						LOG_INFO("File {:s} has thumbnail {:d}: {:d}x{:d}", filename, i, thumbnail.width, thumbnail.height);

						hv::QueryParams query;
						query["name"] = filename;
						while (context.next != 0)
						{
							// Request thumbnail data
							query["offset"] = utils::format("%d", context.next);
							LOG_INFO("Requesting thumbnail data for {:s} at offset {:d}\n", filename, context.next);
							if (!Get("/rr_thumbnail", r, query))
							{
								LOG_ERROR("Failed to get thumbnail data for {:s} at offset {:d}", filename, context.next);
								continue;
							}
							LOG_DBG("Parsing rr_thumbnail response");
							reader.parse(r.body, body);

							if (body.isMember("err") && body["err"].asInt() != 0)
							{
								LOG_ERROR("Failed to get thumbnail data for {:s} at offset {:d}: {:d}",
									  filename,
									  context.next,
									  body["err"].asInt());
								continue;
							}

							if (body.isMember("next"))
							{
								context.next = body["next"].asInt();
								LOG_DBG("Next thumbnail offset: {:d}", context.next);
							}

							LOG_DBG("Decoding thumbnail data");
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
			return false;
		}
		return ret;
	}

	bool Duet::RequestThumbnail(const char* filename, uint32_t offset)
	{
		LOG_DBG("for {:s}, offset={:d}", filename, offset);
		bool ret = true;
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		case CommunicationType::usb:
			SendGcodef("M36.1 P\"%s\" S%d\n", filename, offset);
			break;
		case CommunicationType::network:
		{
#if 1
			hv::QueryParams query;
			query["name"] = filename;
			query["offset"] = utils::format("%d", offset);
			ret = AsyncGet(
				"/rr_thumbnail",
				query,
				[this](const HttpResponsePtr& r) -> bool
				{
					JsonDecoder decoder;
					if (r->status_code != 200)
					{
						LOG_ERROR("HTTP error {:d}: Failed to get thumbnail for file: {:s}",
								  (int)r->status_code,
								  r->body.c_str());
						return false;
					}
					decoder.SetPrefix("thumbnail:");
					decoder.CheckInput((const unsigned char*)r->body.c_str(), r->body.size() + 1);
					return true;
				},
				true);
#endif
			break;
		}
		default:
			return false;
		}
		return ret;
	}

	void Duet::ProcessReply(HttpResponse& reply)
	{
		if (m_config.communicationType != CommunicationType::network)
			return;

		if (reply.body.empty())
		{
			LOG_DBG("Empty reply received");
			return;
		}

		JsonDecoder decoder;
		if (reply.body[0] != '{')
		{
			LOG_DBG("Reply not json: assuming it is a gcode response");

			LOG_DBG("Removing \\r from reply body");
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
				std::string_view line = reply.body.substr(prevPosition, position - prevPosition);
				LOG_DBG("line: {:s}", line);
				prevPosition = position + 1;
				position = reply.body.find("\n", position + 1); // Find the next occurrence, if any
				LOG_VERBOSE("position={:d}, prevPosition={:d}", position, prevPosition);
				if (line.empty())
				{
					LOG_VERBOSE("Skipping empty line");
					continue;
				}
				// Can skip checking the input since we know it's a gcode response
				decoder.ProcessReceivedValue(ref, line.data(), indices);
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
		bool ret = false;

		LOG_DBG("Connecting to Duet, communication type: {:d}", (int)m_config.communicationType);

		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
		{
			LOG_INFO("Opening UART {:s} at {:d}", DEFAULT_UART_PORT, GetBaudRate().rate);
			ret = SerialIo::Init(DEFAULT_UART_PORT, GetBaudRate().internal);
			break;
		}
		case CommunicationType::network:
		{
			LOG_INFO("Connecting to Duet at {:s}", GetBaseUrl().c_str());

			HttpResponse r;
			hv::QueryParams query;
			query["password"] = std::string("\"") + m_config.password + "\"";
			if (useSessionKey)
				query["sessionKey"] = "yes";

			Get("/rr_connect", r, query);

			if (r.status_code != HTTP_STATUS_OK)
			{
				LOG_ERROR("rr_connect failed, returned response {:d}", (int)r.status_code);
				break;
			}

			LOG_VERBOSE("parsing rr_connect response");
			auto body = nlohmann::json::parse(r.body, nullptr, false);
			if (body.is_discarded())
			{
				LOG_ERROR("Failed to parse JSON response from rr_connect");
				break;
			}

			if (body.contains("err") && body["err"].get<int>() != 0)
			{
				LOG_ERROR("rr_connect failed, returned error {:d}", body["err"].get<int>());
				break;
			}

			if (body.contains("sessionTimeout"))
			{
				m_sessionTimeout = body["sessionTimeout"].get<int>();
				m_lastRequestTime = TimeHelper::getCurrentTime();
				LOG_INFO("Duet session timeout set to {:d}", m_sessionTimeout);
			}

			if (body.contains("sessionKey"))
			{
				SetSessionKey(body["sessionKey"].get<unsigned int>());
				LOG_INFO("Duet session key = {:d}", m_sessionKey);
			}
			if (body.contains("isEmulated"))
			{
				SetSessionKey(sm_noSessionKey);
				m_sbcMode = true;
				LOG_INFO("Connected to Duet in SBC mode");
			}
			LOG_INFO("rr_connect succeeded");
			ret = true;
			break;
		}
		case CommunicationType::usb:
		{
			LOG_DBG("Attempting to connect to Duet via USB");
			ret = connectUsbDevice();
			LOG_DBG(ret ? "Connected to USB device" : "Failed to connect to USB device");
			if (ret)
			{
				m_connected = ret; // set connected state so SendGcode actually works
				SendGcode("M575 P0 S4\n");
			}
			break;
		}
		default:
			break;
		}

		m_connected = ret;
		if (m_connected)
		{
			Model::get().post<EventType::Connected>();
		}

		return ret;
	}

	const bool Duet::Disconnect()
	{
		if (!m_connected)
		{
			return true;
		}
		LOG_INFO("Disconnecting from Duet");
		SetStatus(OM::PrinterStatus::connecting);

		bool ret = false;
		switch (m_config.communicationType)
		{
		case CommunicationType::uart:
			SerialIo::Shutdown();
			ret = true;
			break;
		case CommunicationType::network:
		{
			if (m_sessionKey == sm_noSessionKey)
			{
				break;
			}
			HttpResponse r;
			hv::QueryParams query;
			if (!Get("/rr_disconnect", r, query))
			{
				LOG_ERROR("rr_disconnect failed, returned response {:d}", (int)r.status_code);
				ret = false;
				break;
			}
			ret = true;
			break;
		}
		case CommunicationType::usb:
		{
			getCurrentUsbDevice().reset();
			ret = true;
			break;
		}
		default:
			break;
		}

		Reset();
		m_connected = false;
		Model::get().post<EventType::Disconnected>();
		return ret;
	}

	const std::string& Duet::GetBaseUrl() const
	{
		if (!m_config.ipAddress.empty())
		{
			LOG_VERBOSE("Using IP address {:s}", m_config.ipAddress.c_str());
			return m_config.ipAddress;
		}
		LOG_VERBOSE("Using hostname {:s}", m_config.hostname.c_str());
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
		LOG_WARN("Baud rate {:d} not found", baudRateCode);
	}

	void Duet::SetBaudRate(const baudrate_t& baudRate)
	{
		LOG_INFO("Setting baud rate to {:d} ({:d})", baudRate.rate, baudRate.internal);
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
		LOG_WARN("Baud rate {:d} not found", m_config.baudRate);
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
		LOG_DBG("IP address cleared \"{:s}\"", m_config.ipAddress.c_str());
	}

	void Duet::SetHostname(const std::string hostname)
	{
		LOG_DBG("Hostname = {:s}", hostname.c_str());
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
		LOG_INFO("Set Duet hostname to {:s}", m_config.hostname.c_str());
		FILEINFO_CACHE->ClearCache();
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
		LOG_INFO("Set Duet session key = {:d}", m_sessionKey);
	}
} // namespace Comm

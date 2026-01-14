/*
 * SystemHelper.cpp
 *
 *  Created on: 2026-01-14
 *      Author: Andy Everitt
 */

#include "SystemHelper.h"
#include "Debug.h"
#include <cstdlib>
#include <filesystem>
#include <fmt/format.h>

static constexpr std::string_view s_systemCtrlPath =
#if T113
	"/etc/init.d/"
#else
	"./"
#endif
	;

namespace SystemHelper
{
	static inline bool runCommand(const std::string& cmd)
	{
		ZoneScoped;
		LOG_DBG("Running command: {}", cmd);
		int rc = std::system(cmd.c_str());
		if (rc != 0)
		{
			LOG_WARN("Command failed ({}) for: {}", rc, cmd);
			return false;
		}
		return true;
	}

	static constexpr std::string_view getServiceName(const Services service)
	{
		switch (service)
		{
		case Services::DUETSCREEN:
			return "20DuetScreen";
#if DEVELOPER_MODE
		case Services::DUETSCREEN_MONITOR:
			return "21DuetScreenMonitor";
		case Services::ADB:
			return "30adbd";
		case Services::SSH:
			return "50dropbear";
		case Services::SETUP:
			return "02setup";
#endif
		default:
			return "Unknown";
		}
	}

	static constexpr std::string_view getProcessName(const Services service)
	{
		switch (service)
		{
		case Services::DUETSCREEN:
			return "DuetScreen";
#if DEVELOPER_MODE
		case Services::DUETSCREEN_MONITOR:
			return "DuetScreenMonitor";
		case Services::ADB:
			return "adbd";
		case Services::SSH:
			return "dropbear";
		case Services::SETUP:
			return "setup";
#endif
		default:
			return "";
		}
	}

	[[maybe_unused]] static std::string getServicePath(const Services service, const bool enabled)
	{
		return fmt::format("{:s}{:s}{:s}", s_systemCtrlPath, enabled ? "S" : "", getServiceName(service));
	}

	bool startService(Services service)
	{
		ZoneScoped;
		LOG_INFO("Starting service: {}", getServiceName(service));
#if T113
		if (isServiceRunning(service))
		{
			LOG_INFO("Service already running: {}", getServiceName(service));
			return true;
		}

		const auto script = getServicePath(service, isServiceEnabled(service));
		if (!std::filesystem::exists(script))
		{
			LOG_WARN("Start failed: script not found: {}", script);
			return false;
		}
		return runCommand(fmt::format("{} start", script));
#else
		return true;
#endif
	}

	bool stopService(Services service)
	{
		ZoneScoped;
		LOG_INFO("Stopping service: {}", getServiceName(service));
#if T113
		if (!isServiceRunning(service))
		{
			LOG_INFO("Service not running: {}", getServiceName(service));
			return true;
		}

		const auto script = getServicePath(service, isServiceEnabled(service));
		if (!std::filesystem::exists(script))
		{
			LOG_WARN("Stop failed: script not found: {}", script);
			return false;
		}
		return runCommand(fmt::format("{} stop", script));
#else
		return true;
#endif
	}

	bool restartService(Services service)
	{
		ZoneScoped;
		LOG_INFO("Restarting service: {}", getServiceName(service));
#if T113
		const auto script = getServicePath(service, isServiceEnabled(service));
		if (!std::filesystem::exists(script))
		{
			LOG_WARN("Restart failed: script not found: {}", script);
			return false;
		}
		return runCommand(fmt::format("{} restart", script));
#else
		return true;
#endif
	}

	bool enableService(Services service)
	{
		ZoneScoped;
		LOG_INFO("Enabling service: {}", getServiceName(service));
#if T113
		const auto disabled = getServicePath(service, /*enabled*/ false);
		const auto enabled = getServicePath(service, /*enabled*/ true);
		std::error_code ec;
		if (std::filesystem::exists(enabled))
		{
			return true; // already enabled
		}
		if (!std::filesystem::exists(disabled))
		{
			LOG_WARN("Enable failed: script not found: {}", disabled);
			return false;
		}
		std::filesystem::rename(disabled, enabled, ec);
		if (ec)
		{
			LOG_WARN("Enable rename failed: {} -> {} ({})", disabled, enabled, ec.message());
			return false;
		}
		return true;
#else
		return true;
#endif
	}

	bool disableService(Services service)
	{
		ZoneScoped;
		LOG_INFO("Disabling service: {}", getServiceName(service));
#if T113
		const auto enabled = getServicePath(service, /*enabled*/ true);
		const auto disabled = getServicePath(service, /*enabled*/ false);
		std::error_code ec;
		if (std::filesystem::exists(disabled))
		{
			return true; // already disabled
		}
		if (!std::filesystem::exists(enabled))
		{
			LOG_WARN("Disable failed: script not found: {}", enabled);
			return false;
		}
		std::filesystem::rename(enabled, disabled, ec);
		if (ec)
		{
			LOG_WARN("Disable rename failed: {} -> {} ({})", enabled, disabled, ec.message());
			return false;
		}
		return true;
#else
		return true;
#endif
	}

	bool isServiceRunning(Services service)
	{
		ZoneScoped;
		LOG_DBG("Checking if service is running: {}", getServiceName(service));
#if T113
		const auto proc = getProcessName(service);
		if (proc.empty())
		{
			return false;
		}
		// Use pidof to check if the process is running
		const auto cmd = fmt::format("pidof {} > /dev/null 2>&1", proc);
		int rc = std::system(cmd.c_str());
		return rc == 0;

#else
		return true;
#endif
	}

	bool isServiceEnabled(Services service)
	{
		ZoneScoped;
		LOG_DBG("Checking if service is enabled: {}", getServiceName(service));
#if T113
		const auto enabled = getServicePath(service, /*enabled*/ true);
		if (std::filesystem::exists(enabled))
		{
			return true;
		}

		if (std::filesystem::exists(getServicePath(service, /*enabled*/ false)))
		{
			return false;
		}

		LOG_FATAL_THROW("Bad service name: '{}'", getServiceName(service));
#else
		return true;
#endif
	}

} // namespace SystemHelper

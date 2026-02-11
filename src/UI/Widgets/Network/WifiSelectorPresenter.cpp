/*
 * WifiSelectorPresenter.cpp
 *
 *  Created on: 2025-12-16
 *      Author: Andy Everitt
 */

#include "WifiSelectorPresenter.h"
#include "Debug.h"
#include "WifiSelector.h"
#include "i18n/i18n.h"
#include "utils/NetworkHelper.h"

namespace UI
{
	WifiSelectorPresenter::~WifiSelectorPresenter()
	{
		ZoneScoped;
		if (m_connectThread.joinable())
		{
			m_connectThread.request_stop();
			m_connectThread.join();
		}
		if (m_scanThread.joinable())
		{
			m_scanThread.request_stop();
			m_scanThread.join();
		}
	}

	void WifiSelectorPresenter::connectAsync(std::string ssid, std::string password, bool hasPassword)
	{
		ZoneScoped;
		// Prevent overlapping connect attempts
		if (m_connecting.load())
			return;

		// Show "connecting" status immediately on the UI thread
		getView()->setStatusMessage(_("settings.network.status_connecting", ssid));

		// If a previous connect thread finished, clean it up
		if (m_connectThread.joinable())
			m_connectThread.join();

		m_connecting.store(true);

		m_connectThread = std::jthread(
			[this, ssid = std::move(ssid), password = std::move(password), hasPassword](std::stop_token st)
			{
				tracy::SetThreadName("WifiConnectThread");
				auto result = hasPassword ? NetworkHelper::connect(ssid, password) : NetworkHelper::connect(ssid);

				m_connecting.store(false);

				if (st.stop_requested())
					return;

				showConnectionResult(ssid, result);
				refresh();
			});
	}

	void WifiSelectorPresenter::refresh()
	{
		ZoneScoped;
		if (!getView())
			return;
		std::vector<WiFiNetwork> networks = NetworkHelper::scanWiFiNetworks();
		{
			UI_LOCK();
			getView()->setNetworkCount(networks.size());
			for (size_t i = 0; i < networks.size(); ++i)
			{
				const auto& network = networks[i];
				getView()->setNetworkDetails(
					i, network.ssid, network.signal_level, network.isKnown(), network.connected);
			}
		}
		getView()->setIpAddress(NetworkHelper::getIpAddress());
	}

	void WifiSelectorPresenter::connectToNetwork(std::string_view ssid)
	{
		ZoneScoped;
		connectAsync(std::string{ssid});
	}

	void WifiSelectorPresenter::connectToNetwork(std::string_view ssid, std::string_view password)
	{
		ZoneScoped;
		connectAsync(std::string{ssid}, std::string{password}, true);
	}

	void WifiSelectorPresenter::disconnectFromNetwork()
	{
		ZoneScoped;
		NetworkHelper::disconnect();
		{
			UI_LOCK();
			getView()->setStatusMessage(_("settings.network.status_disconnected"));
		}
		refresh();
	}

	void WifiSelectorPresenter::forgetNetwork(std::string_view ssid)
	{
		ZoneScoped;
		NetworkHelper::forgetNetwork(ssid);
		refresh();
	}

	void WifiSelectorPresenter::showConnectionResult(std::string_view ssid, NetworkHelper::ConnectResult result)
	{
		ZoneScoped;
		UI_LOCK();
		switch (result)
		{
		case NetworkHelper::ConnectResult::Success:
			getView()->setStatusMessage(_("settings.network.status_connected", ssid));
			break;
		case NetworkHelper::ConnectResult::AuthFailure:
			getView()->setStatusMessage(_("settings.network.status_auth_failure", ssid));
			break;
		case NetworkHelper::ConnectResult::Timeout:
			getView()->setStatusMessage(_("settings.network.status_timeout", ssid));
			break;
		case NetworkHelper::ConnectResult::NetworkNotFound:
			getView()->setStatusMessage(_("settings.network.status_not_found", ssid));
			break;
		case NetworkHelper::ConnectResult::Error:
			getView()->setStatusMessage(_("settings.network.status_error", ssid));
			break;
		}
	}

	void WifiSelectorPresenter::onInit()
	{
		ZoneScoped;
		LOG_DBG("Initialising WiFi scan thread");
		if (!m_scanThread.joinable())
		{
			m_scanThread = std::jthread(
				[this](std::stop_token st)
				{
					tracy::SetThreadName("WifiScanThread");
					std::unique_lock<std::mutex> lk(m_scanMutex);
					for (;;)
					{
						m_scanCv.wait(lk, st, [this] { return m_scanActive.load(); });
						if (st.stop_requested())
							break;

						lk.unlock();
						refresh();
						lk.lock();

						// Wait for interval, or pause/stop
						m_scanCv.wait_for(lk, st, std::chrono::seconds(3), [this] { return !m_scanActive.load(); });
						if (st.stop_requested())
							break;
						// If paused, loop back to wait for activation again; if timed out, refresh again
					}
				});
		}
	}

	void WifiSelectorPresenter::onActivate()
	{
		ZoneScoped;
		LOG_DBG("Activating WiFi scan thread");
		{
			std::lock_guard<std::mutex> lk(m_scanMutex);
			m_scanActive.store(true);
		}
		m_scanCv.notify_all();
	}

	void WifiSelectorPresenter::onDeactivate()
	{
		ZoneScoped;
		LOG_DBG("Deactivating WiFi scan thread");
		{
			std::lock_guard<std::mutex> lk(m_scanMutex);
			m_scanActive.store(false);
		}
		m_scanCv.notify_all();
	}

} // namespace UI

/*
 * WifiSelectorPresenter.cpp
 *
 *  Created on: 2025-12-16
 *      Author: Andy Everitt
 */

#include "WifiSelectorPresenter.h"
#include "Debug.h"
#include "WifiSelector.h"
#include "utils/NetworkHelper.h"

namespace UI
{
	WifiSelectorPresenter::~WifiSelectorPresenter()
	{
		ZoneScoped;
		if (m_scanThread.joinable())
		{
			m_scanThread.request_stop();
			m_scanThread.join();
		}
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
		NetworkHelper::connect(ssid);
		refresh();
	}

	void WifiSelectorPresenter::connectToNetwork(std::string_view ssid, std::string_view password)
	{
		ZoneScoped;
		NetworkHelper::connect(ssid, password);
		refresh();
	}

	void WifiSelectorPresenter::disconnectFromNetwork()
	{
		ZoneScoped;
		NetworkHelper::disconnect();
		refresh();
	}

	void WifiSelectorPresenter::forgetNetwork(std::string_view ssid)
	{
		ZoneScoped;
		NetworkHelper::forgetNetwork(ssid);
		refresh();
	}

	void WifiSelectorPresenter::onInit()
	{
		ZoneScoped;
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
		{
			std::lock_guard<std::mutex> lk(m_scanMutex);
			m_scanActive.store(true);
		}
		m_scanCv.notify_all();
	}

	void WifiSelectorPresenter::onDeactivate()
	{
		ZoneScoped;
		{
			std::lock_guard<std::mutex> lk(m_scanMutex);
			m_scanActive.store(false);
		}
		m_scanCv.notify_all();
	}

} // namespace UI

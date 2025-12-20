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
		if (m_scanThread.joinable())
		{
			m_scanThread.request_stop();
			m_scanThread.join();
		}
	}

	void WifiSelectorPresenter::refresh()
	{
		UI_LOCK();
		if (!getView())
			return;
		std::vector<WiFiNetwork> networks = NetworkHelper::scanWiFiNetworks();
		getView()->setNetworkCount(networks.size());
		for (size_t i = 0; i < networks.size(); ++i)
		{
			const auto& network = networks[i];
			getView()->setNetworkDetails(i, network.ssid, network.signal_level, network.isKnown(), network.connected);
		}
		getView()->setIpAddress(NetworkHelper::getIpAddress());
	}

	void WifiSelectorPresenter::connectToNetwork(std::string_view ssid)
	{
		NetworkHelper::connect(ssid);
		refresh();
	}

	void WifiSelectorPresenter::connectToNetwork(std::string_view ssid, std::string_view password)
	{
		NetworkHelper::connect(ssid, password);
		refresh();
	}

	void WifiSelectorPresenter::disconnectFromNetwork()
	{
		NetworkHelper::disconnect();
		refresh();
	}

	void WifiSelectorPresenter::onInit()
	{
		if (!m_scanThread.joinable())
		{
			m_scanThread = std::jthread(
				[this](std::stop_token st)
				{
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
		{
			std::lock_guard<std::mutex> lk(m_scanMutex);
			m_scanActive.store(true);
		}
		m_scanCv.notify_all();
	}

	void WifiSelectorPresenter::onDeactivate()
	{
		{
			std::lock_guard<std::mutex> lk(m_scanMutex);
			m_scanActive.store(false);
		}
		m_scanCv.notify_all();
	}

} // namespace UI

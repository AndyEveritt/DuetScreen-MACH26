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
		m_runScanThread = false;
		if (m_scanThread.joinable())
			m_scanThread.join();
	}

	void WifiSelectorPresenter::refresh()
	{
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

	void WifiSelectorPresenter::onInit() {}

	void WifiSelectorPresenter::onActivate()
	{
		refresh();

		if (m_scanThread.joinable())
			m_scanThread.join();

		m_runScanThread = true;
		m_scanThread = std::thread(
			[this]()
			{
				while (m_runScanThread)
				{
					refresh();
					std::this_thread::sleep_for(std::chrono::seconds(3));
				}
			});
	}

	void WifiSelectorPresenter::onDeactivate()
	{
		m_runScanThread = false;
	}

} // namespace UI

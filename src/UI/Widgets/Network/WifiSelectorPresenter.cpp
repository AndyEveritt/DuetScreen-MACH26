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
	void WifiSelectorPresenter::refresh()
	{
		if (!getView())
			return;
		std::vector<WiFiNetwork> networks = NetworkHelper::scanWiFiNetworks();
		getView()->setNetworkCount(networks.size());
		for (size_t i = 0; i < networks.size(); ++i)
		{
			const auto& network = networks[i];
			getView()->setNetworkDetails(i, network.ssid, network.signal_level, network.id != -1, network.connected);
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

	void WifiSelectorPresenter::disconnectFromNetwork() {}

	void WifiSelectorPresenter::onInit()
	{
		m_scanTimer = lv_timer_create(
			[](lv_timer_t* timer)
			{
				auto* presenter = static_cast<WifiSelectorPresenter*>(lv_timer_get_user_data(timer));
				if (presenter)
					presenter->refresh();
			},
			3000,
			this);
		lv_timer_pause(m_scanTimer);
	}

	void WifiSelectorPresenter::onActivate()
	{
		refresh();
		if (m_scanTimer)
			lv_timer_resume(m_scanTimer);
	}

	void WifiSelectorPresenter::onDeactivate()
	{
		if (m_scanTimer)
			lv_timer_pause(m_scanTimer);
	}

} // namespace UI

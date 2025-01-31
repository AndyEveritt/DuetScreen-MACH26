#include "SettingsPresenter.h"
#include "SettingsView.h"
#include "utils/NetworkHelper.h"

namespace UI
{
	void NetworkSettingsPresenter::connectToNetwork(const std::string& ssid)
	{
		NetworkHelper::connect(ssid);
	}

	void NetworkSettingsPresenter::connectToNetwork(const std::string& ssid, const std::string& password)
	{
		NetworkHelper::connect(ssid, password);
	}

	void NetworkSettingsPresenter::forgetNetwork(const std::string& ssid)
	{
		NetworkHelper::forgetNetwork(ssid);
	}

	void NetworkSettingsPresenter::scanWifi()
	{
#if SIMULATION
		std::vector<WiFiNetwork> networks = {
			{"Network 1", 100, 1}, {"Network 2", 75, 2}, {"Network 3", 50, 3}, {"Network 4", 25, -1}};
#else
		std::vector<WiFiNetwork> networks = NetworkHelper::scanWiFiNetworks();
#endif

		m_view->setNetworkCount(networks.size());
		for (size_t i = 0; i < networks.size(); ++i)
		{
			m_view->setNetworkDetails(i, networks[i].ssid, networks[i].signal_level, networks[i].id != -1);
		}
	}

	void NetworkSettingsPresenter::onActivate() {}
} // namespace UI

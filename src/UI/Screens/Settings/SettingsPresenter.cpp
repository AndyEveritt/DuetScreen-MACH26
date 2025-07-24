#include "SettingsPresenter.h"
#include "Pins.h"
#include "SettingsView.h"
#include "utils/GpioHelper.h"
#include "utils/NetworkHelper.h"
#include "utils/StorageHelper.h"
#include <fstream>

namespace UI
{
	void SettingsPresenter::setUsbMode(UsbMode mode)
	{
		LOG_DBG("Setting USB mode to {:d}", static_cast<int>(mode));
		switch (mode)
		{
		case UsbMode::Host:
			setUsbHost(true);
			setUsbMux(true);
			setUsbState(true);
			break;
		case UsbMode::Device:
			setUsbHost(false);
			setUsbMux(true);
			setUsbState(false);
			break;
		case UsbMode::InternalWiFi:
			setUsbHost(true);
			setUsbMux(false);
			setUsbState(false);
			break;
		}
		LOG_INFO("USB mode set to {:d}", static_cast<int>(mode));
		StorageHelper::setData(ID_USB_MODE, static_cast<int>(mode));
	}

	void SettingsPresenter::setUsbHost(bool host)
	{
		std::ofstream ofs(USB_OTG_ROLE_PATH);
		ofs << (host ? "usb_host" : "usb_device");
		ofs.close();
	}

	void SettingsPresenter::setUsbMux(bool usbc)
	{
		GpioHelper::setPinValue(GPIO_USB_SELECT, usbc ? 1 : 0);
	}

	void SettingsPresenter::setUsbState(bool state)
	{
		GpioHelper::setPinValue(GPIO_USB_STATE, state ? 1 : 0);
	}

	void SettingsPresenter::onInit()
	{
		// Set the USB mode based on the stored value
		int usbMode = StorageHelper::getData(ID_USB_MODE, 0);
		setUsbMode(static_cast<UsbMode>(usbMode));
	}

	void NetworkSettingsPresenter::setWifiEnabled(bool enabled)
	{
		NetworkHelper::enable(enabled);
		scanWifi();
	}

	void NetworkSettingsPresenter::connectToNetwork(const std::string& ssid)
	{
		NetworkHelper::connect(ssid);
		scanWifi();
	}

	void NetworkSettingsPresenter::connectToNetwork(const std::string& ssid, std::string_view password)
	{
		NetworkHelper::connect(ssid, password);
		scanWifi();
	}

	void NetworkSettingsPresenter::forgetNetwork(const std::string& ssid)
	{
		NetworkHelper::forgetNetwork(ssid);
		scanWifi();
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
			m_view->setNetworkDetails(
				i, networks[i].ssid, networks[i].signal_level, networks[i].id != -1, networks[i].connected);
		}
		m_view->setIpAddress(NetworkHelper::getIpAddress());
	}

	void NetworkSettingsPresenter::onActivate()
	{
		m_view->setEnabled(NetworkHelper::isEnabled());
		m_view->setIpAddress(NetworkHelper::getIpAddress());
	}
} // namespace UI

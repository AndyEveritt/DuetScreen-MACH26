#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class SettingsView;
	class GeneralSettings;
	class ConnectionSettings;
	class DisplaySettings;
	class DeveloperSettings;
	class NetworkSettingsView;

	class SettingsPresenter : public Presenter<SettingsView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(SettingsPresenter, SettingsView)

	  private:
	};

	class GeneralSettingsPresenter : public Presenter<GeneralSettings>
	{
	  public:
		PRESENTER_CONSTRUCTOR(GeneralSettingsPresenter, GeneralSettings)

	  private:
	};

	class ConnectionSettingsPresenter : public Presenter<ConnectionSettings>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ConnectionSettingsPresenter, ConnectionSettings)

	  private:
		void onInit() override;
	};

	class DisplaySettingsPresenter : public Presenter<DisplaySettings>
	{
	  public:
		PRESENTER_CONSTRUCTOR(DisplaySettingsPresenter, DisplaySettings)

	  private:
	};

	class DeveloperSettingsPresenter : public Presenter<DeveloperSettings>
	{
	  public:
		PRESENTER_CONSTRUCTOR(DeveloperSettingsPresenter, DeveloperSettings)

		// Actions
		void startHardwareTest();

	  private:
	};

#if 0
	class NetworkSettingsPresenter : public Presenter<NetworkSettingsView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(NetworkSettingsPresenter, NetworkSettingsView)

		// Actions
		void setWifiEnabled(bool enabled);
		void scanWifi();
		void connectToNetwork(std::string_view ssid);
		void connectToNetwork(std::string_view ssid, std::string_view password);
		void forgetNetwork(std::string_view ssid);
		void refresh();

	  private:
		void onInit() override;
		void onActivate() override;
		void onDeactivate() override;

		lv_timer_t* m_scanTimer = nullptr;
	};
#endif
} // namespace UI

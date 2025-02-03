#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class SettingsView;
	class NetworkSettingsView;

	class SettingsPresenter : public Presenter<SettingsView>
	{
	  public:
		using Presenter::Presenter;

	  private:
	};

	class NetworkSettingsPresenter : public Presenter<NetworkSettingsView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		void setWifiEnabled(bool enabled);
		void scanWifi();
		void connectToNetwork(const std::string& ssid);
		void connectToNetwork(const std::string& ssid, const std::string& password);
		void forgetNetwork(const std::string& ssid);

	  private:
		void onActivate() override;
	};
} // namespace UI

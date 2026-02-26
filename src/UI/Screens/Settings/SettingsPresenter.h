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
		void refreshCacheInfo();

		// Trigger GitHub latest release download + upgrade
		void upgradeFromGithubLatest();

	  private:
		void onActivate() override;
	};
} // namespace UI

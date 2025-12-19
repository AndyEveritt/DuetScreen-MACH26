/*
 * WifiSelectorPresenter.h
 *
 *  Created on: 2025-12-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"
#include <string>
#include <thread>

namespace UI
{
	class WifiSelector;

	class WifiSelectorPresenter : public Presenter<WifiSelector>
	{
	  public:
		PRESENTER_CONSTRUCTOR(WifiSelectorPresenter, WifiSelector);
		~WifiSelectorPresenter() override;

		// Actions
		void refresh();
		void connectToNetwork(std::string_view ssid);
		void connectToNetwork(std::string_view ssid, std::string_view password);
		void disconnectFromNetwork();

	  protected:
		void onInit() override;
		void onActivate() override;
		void onDeactivate() override;

		void onConnect() override {}
		void onDisconnect() override {}

		std::thread m_scanThread;
		bool m_runScanThread = false;
	};
} // namespace UI

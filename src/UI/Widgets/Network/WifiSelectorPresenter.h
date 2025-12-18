/*
 * WifiSelectorPresenter.h
 *
 *  Created on: 2025-12-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"
#include <string>

namespace UI
{
	class WifiSelector;

	class WifiSelectorPresenter : public Presenter<WifiSelector>
	{
	  public:
		PRESENTER_CONSTRUCTOR(WifiSelectorPresenter, WifiSelector);

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

		lv_timer_t* m_scanTimer = nullptr;
	};
} // namespace UI

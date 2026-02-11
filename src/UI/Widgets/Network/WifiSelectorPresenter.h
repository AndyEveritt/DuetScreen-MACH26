/*
 * WifiSelectorPresenter.h
 *
 *  Created on: 2025-12-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"
#include "utils/NetworkHelper.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
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
		void forgetNetwork(std::string_view ssid);

	  protected:
		void onInit() override;
		void onActivate() override;
		void onDeactivate() override;

		void onConnect() override {}
		void onDisconnect() override {}

	  private:
		/// Push a ConnectResult to the view as a user-visible status message.
		void showConnectionResult(std::string_view ssid, NetworkHelper::ConnectResult result);

		/// Run a connect operation on m_connectThread, showing status immediately.
		void connectAsync(std::string ssid, std::string password = {}, bool hasPassword = false);

		std::jthread m_scanThread;
		std::atomic<bool> m_scanActive{false};
		std::mutex m_scanMutex;
		std::condition_variable_any m_scanCv;

		std::jthread m_connectThread;
		std::atomic<bool> m_connecting{false};
	};
} // namespace UI

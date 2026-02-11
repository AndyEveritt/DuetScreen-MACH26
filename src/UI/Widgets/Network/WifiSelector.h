/*
 * WifiSelector.h
 *
 *  Created on: 2025-12-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/List/List.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"
#include "WifiSelectorPresenter.h"

namespace UI
{
	class WifiSelector : public View<WifiSelectorPresenter>
	{
	  public:
		WifiSelector(const std::string& name, LvObj& parent);

		// Data API used by presenter
		void setNetworkCount(size_t count);
		void setNetworkDetails(size_t index, const std::string& ssid, int32_t signalLevel, bool known, bool connected);
		void setIpAddress(std::string_view ip_address);
		void setStatusMessage(std::string_view message);

		void setKeyboard(LvKeyboard* keyboard);

	  private:
		class WifiListItem : public ListItem
		{
		  public:
			WifiListItem(size_t index, LvObj& parent);

			void setSsid(const std::string& ssid);
			void setSignalLevel(int32_t levelDbm);
			void setKnown(bool known);
			void setConnected(bool connected);

			const std::string& getSsid() const { return m_ssid; }
			bool isKnown() const { return m_known; }
			bool isConnected() const { return m_isConnected; }

			Button& getConnectButton() { return m_connectBtn; }
			Button& getForgetButton() { return m_forgetBtn; }

		  private:
			LvLabel m_ssidLabel{"ssid", getRoot()};
			LvLabel m_signalLabel{"signal", getRoot()};
			Button m_connectBtn{"connect", getRoot()};
			Button m_forgetBtn{"forget", getRoot()};

			std::string m_ssid;
			int32_t m_signal = 0;
			bool m_known = false;
			bool m_isConnected = false;
		};

		void onInit() override;

		void openPasswordModal(const std::string& ssid);

		LvContainer m_header{"header", getRoot()};
		LvLabel m_statusLabel{"status_message", m_header};
		LvLabel m_ipAddress{"ip_address", m_header};
		List<WifiListItem> m_list{"wifi_list", getRoot()};
		Button m_refresh{"refresh", m_list.getHeader(), "Refresh"};

		// Simple password modal composed with MessageBox + TextBox
		Modal<MessageBox> m_passwordModal{"wifi_password_modal", getRoot(), layout_t(40, 20, 80, LV_SIZE_CONTENT)};
		TextBox m_passwordInput{"wifi_password_input", m_passwordModal.getBody()};

		LvKeyboard* m_keyboard = nullptr;
		std::string m_pendingSsid;

	  private:
	};
} // namespace UI

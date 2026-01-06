/*
 * WifiSelector.cpp
 *
 *  Created on: 2025-12-16
 *      Author: Andy Everitt
 */

#include "WifiSelector.h"
#include "Debug.h"
#include "UI/Screens/Home/HomeView.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"

namespace UI
{
	WifiSelector::WifiSelector(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setSize(LV_PCT(100), LV_SIZE_CONTENT);

		setIpAddress("-");

		m_list.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_list.setTitle(_("settings.network.available_wifi"));
		m_list.getTitle().setFlexGrow(1);
		m_refresh.setText(_("settings.network.refresh"));
		m_refresh.addClickedCallback([this](lv_event_t*) { getPresenter()->refresh(); });

		// Setup password modal
		m_passwordModal.setTitle(_("settings.network.enter_password_title"));
		// m_passwordModal.setText("");
		m_passwordModal.okVisible(true);
		m_passwordModal.cancelVisible(true);
		m_passwordModal.getBodyTextCont().hide();
		// m_passwordModal.updateVisibility();

		m_passwordInput.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_passwordInput.setPlaceholderText(_("settings.network.enter_password_hint"));
		m_passwordInput.setPasswordMode(true);
		m_passwordModal.setCloseCallback(
			[this]()
			{
				m_passwordModal.close();
				if (auto kb = m_passwordInput.getKeyboard())
					kb->hide();
			});
		m_passwordModal.setOkCallback([this]()
									  { getPresenter()->connectToNetwork(m_pendingSsid, m_passwordInput.getText()); });
		m_passwordInput.getTextarea().addEventCallback(
			[this](lv_event_t* e)
			{
				if (auto kb = m_passwordInput.getKeyboard())
				{
					auto code = lv_event_get_code(e);
					switch (code)
					{
					case LV_EVENT_CLICKED:
						m_passwordModal.setAlign(LV_ALIGN_TOP_MID, 0, 50);
						kb->show(true);
						break;
					case LV_EVENT_DEFOCUSED:
					case LV_EVENT_READY:
						m_passwordModal.setAlign(LV_ALIGN_CENTER, 0, 0);
						kb->hide();
						break;
					default:
						break;
					}
				}
			},
			LV_EVENT_ALL);
	}

	void WifiSelector::setNetworkCount(size_t count)
	{
		m_list.setItemCount(count,
							[this](size_t index, LvObj& parent)
							{
								auto item = std::make_unique<WifiListItem>(index, parent);
								item->getConnectButton().addEventCallback(
									[this, index, &itemRef = *item](lv_event_t*)
									{
										const std::string& ssid = itemRef.getSsid();
										if (itemRef.isConnected())
										{
											getPresenter()->disconnectFromNetwork();
										}
										if (itemRef.isKnown())
										{
											getPresenter()->connectToNetwork(ssid);
										}
										else
										{
											openPasswordModal(ssid);
											m_passwordInput.getTextarea().sendEvent(LV_EVENT_CLICKED);
										}
									},
									LV_EVENT_CLICKED);
								return item;
							});
	}

	void WifiSelector::setNetworkDetails(
		size_t index, const std::string& ssid, int32_t signalLevel, bool known, bool connected)
	{
		auto* item = m_list.getItem(index);
		if (!item)
			return;
		item->setSsid(ssid);
		item->setSignalLevel(signalLevel);
		item->setKnown(known);
		item->setConnected(connected);
	}

	void WifiSelector::setIpAddress(std::string_view ip_address)
	{
		m_ipAddress.setText(_("settings.network.ip_address", ip_address));
	}

	void WifiSelector::setKeyboard(LvKeyboard* keyboard)
	{
		m_keyboard = keyboard;
		m_passwordInput.setKeyboard(m_keyboard);
	}

	WifiSelector::WifiListItem::WifiListItem(size_t index, LvObj& parent)
		: ListItem(index, parent)
	{
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_ssidLabel.setText("-");
		m_ssidLabel.setLongMode(LV_LABEL_LONG_SCROLL_CIRCULAR);
		m_ssidLabel.setFlexGrow(2);

		m_signalLabel.setText("0 dBm");
		m_signalLabel.setFlexGrow(1);

		m_statusLabel.setText(_("settings.network.known"));
		m_statusLabel.setFlexGrow(1);

		m_connectBtn.setFlexGrow(1);
		m_connectBtn.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_connectBtn.setMinWidth(LV_SIZE_CONTENT);
		m_connectBtn.setText(_("settings.network.connect"));

		m_forgetBtn.setFlexGrow(1);
		m_forgetBtn.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_forgetBtn.setMinWidth(LV_SIZE_CONTENT);
		m_forgetBtn.setText(_("settings.network.forget"));
	}

	void WifiSelector::WifiListItem::setSsid(const std::string& ssid)
	{
		m_ssid = ssid;
		m_ssidLabel.setText(m_ssid);
	}

	void WifiSelector::WifiListItem::setSignalLevel(int32_t levelDbm)
	{
		m_signal = levelDbm;
		m_signalLabel.setText(fmt::format("{:d} dBm", m_signal));
	}

	void WifiSelector::WifiListItem::setKnown(bool known)
	{
		m_known = known;
		m_statusLabel.setVisible(m_known);
		m_forgetBtn.setVisible(m_known);
	}

	void WifiSelector::WifiListItem::setConnected(bool connected)
	{
		m_isConnected = connected;
		if (connected)
		{
			m_statusLabel.setText(_("settings.network.connected"));
			m_connectBtn.setText(_("settings.network.disconnect"));
		}
		else
		{
			m_connectBtn.setText(m_known ? _("settings.network.connect") : _("settings.network.join"));
		}
	}

	void WifiSelector::onInit()
	{
		m_passwordModal.setParent(HomeView::instance().getMainWindow());
	}

	void WifiSelector::openPasswordModal(const std::string& ssid)
	{
		m_pendingSsid = ssid;
		m_passwordInput.setText("");
		m_passwordModal.setTitle(_("settings.network.password_for", ssid));
		openModal(&m_passwordModal);
	}
} // namespace UI

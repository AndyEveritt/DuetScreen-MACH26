#include "HomePresenter.h"
#include "HomeView.h"

#include "Hardware/Duet.h"
#include "Hardware/Usb.h"
#include "ObjectModel/Heat.h"
#include "ObjectModel/Sensor.h"
#include "UI/Core/Navigation.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"
#include "utils/UpgradeHelper.h"
#include <algorithm>
#include <regex>

namespace UI
{
	void HomePresenter::onInit()
	{
		ZoneScoped;
		registerEventListener<EventType::UpdateAvailable>(this, &HomePresenter::newUpdateAvailable);
		registerEventListener<EventType::AxesData>(this, &HomePresenter::newAxesData);
		registerEventListener<EventType::Response>(this, &HomePresenter::newResponse);
		registerEventListener<EventType::Alert>(this, &HomePresenter::newAlertData);

		USB::UsbMonitor::getInstance().registerCallback(
			[this](const std::string& path, bool mounted)
			{
				if (mounted)
				{
					return;
				}

				if (m_updateFile.rfind(path, 0) != 0)
				{
					return;
				}
				m_updateFile.clear();
				m_view->showUpdatePrompt(false);
			});
	}

	void HomePresenter::onActivate()
	{
		ZoneScoped;
		newAxesData();
	}

	void HomePresenter::clear()
	{
		ZoneScoped;
		UI_LOCK();
		m_view->clear();
		m_alertAxes.clear();
	}

	void HomePresenter::update()
	{
		ZoneScoped;
		UpgradeHelper::upgradeFromUSB(m_updateFile);
	}

	void HomePresenter::newUpdateAvailable(const std::string& file)
	{
		ZoneScoped;
		m_updateFile = file.c_str();
		m_view->showUpdatePrompt(true);
	}

	void HomePresenter::newAxesData()
	{
		ZoneScoped;
		// Alert jog axes
		for (size_t i = 0; i < m_alertAxes.size(); i++)
		{
			auto axis = OM::Move::GetAxisByLetter(m_alertAxes[i]);
			if (axis == nullptr)
				continue;
			m_view->m_alert.setJogAxisPosition(i, axis->userPosition);
			m_view->m_alert.setJogAxisEnabled(i, axis->homed);
		}
	}

	void HomePresenter::newResponse(const ResponseType type, const std::string& resp)
	{
		ZoneScoped;
		UI_LOCK();
		if (m_view->m_consoleView.isVisible())
		{
			return;
		}

		if (resp.empty())
		{
			return;
		}

		if (static_cast<int>(type) < StorageHelper::getData<int>(ID_NOTIFICATION_LEVEL, DEFAULT_NOTIFICATION_LEVEL))
		{
			return;
		}

		auto& msgBox = m_view->createMessageBox();
		msgBox->setText(resp);
		msgBox->setCancelCallback(
			[this]()
			{
				UI_LOCK();
				m_view->popMessageBox();
				if (m_view->getMessageBoxCount() > 0)
				{
					auto msgBox = m_view->getMessageBox(0);
					if (!msgBox)
						return;
					msgBox->show();
					msgBox->setTimeout(StorageHelper::getData(ID_NOTIFICATION_TIMEOUT, DEFAULT_NOTIFICATION_TIMEOUT));
				}
			});

		msgBox->setType(type);
		switch (type)
		{
		case ResponseType::SUCCESS:
			msgBox->setTitle(_("msgbox.response_success"));
			break;
		case ResponseType::INFO:
			msgBox->setTitle(_("msgbox.response_info"));
			break;
		case ResponseType::WARNING:
			msgBox->setTitle(_("msgbox.response_warning"));
			break;
		case ResponseType::ERROR:
			msgBox->setTitle(_("msgbox.response_error"));
			break;
		default:
			LOG_WARN("Unknown response type {:d}", static_cast<int>(type));
			break;
		}

		msgBox->setCancelBtnText(_("common.close"));
		msgBox->setOkBtnText(_("msgbox.open_console"));
		msgBox->setOkCallback(
			[this]()
			{
				UI_LOCK();
				m_view->clearMessageBoxes();
				openScreen(&m_view->m_consoleView);
			});
		msgBox->okVisible(true);
		msgBox->cancelVisible(true);
		msgBox->progressVisible(true);
		if (m_view->getMessageBoxCount() == 1)
		{
			msgBox->show();
			msgBox->setTimeout(StorageHelper::getData(ID_NOTIFICATION_TIMEOUT, DEFAULT_NOTIFICATION_TIMEOUT));
		}
		msgBox->setProgressCallback([](MessageBox* msgBox) -> uint32_t { return msgBox->getTimeOutPercentage(); });
		if (m_view->m_alert.isVisible())
		{
			msgBox->hide();
		}
	}

	void HomePresenter::newAlertData(const OM::Alert& alert)
	{
		ZoneScoped;
		auto& modalAlert = m_view->m_alert;

		// First clear any existing alert state
		m_alertAxes.clear();

		if (alert.mode == OM::Alert::Mode::None)
		{
			if (modalAlert.isVisible())
			{
				modalAlert.AlertMessageBox::close();
				modalAlert.close();
			}
			return;
		}

		// Configure the message box before showing it to prevent partial updates
		modalAlert.clear();
		modalAlert.setTitle(alert.title.c_str());
		modalAlert.setText(alert.text.c_str());
		modalAlert.setChoiceCount(alert.choices_count);

		for (size_t i = 0; i < alert.choices_count; i++)
		{
			modalAlert.setChoice(i, alert.choices[i].c_str());
		}

		modalAlert.setMode(alert.mode);
		modalAlert.setBlocking(true);

		// Set up close callback first to ensure proper cleanup
		modalAlert.setCloseCallback(
			[this]()
			{
				UI_LOCK();
				m_view->m_alert.close();
				if (m_view->getMessageBoxCount() > 0)
				{
					auto msgBox = m_view->getMessageBox(0);
					if (msgBox) // Add null check
					{
						msgBox->show();
						msgBox->setTimeout(
							StorageHelper::getData(ID_NOTIFICATION_TIMEOUT, DEFAULT_NOTIFICATION_TIMEOUT));
					}
				}
			});

		// Handle mode-specific setup
		uint32_t seq = alert.seq;
		switch (alert.mode)
		{
		case OM::Alert::Mode::Info:
		case OM::Alert::Mode::InfoClose:
		case OM::Alert::Mode::InfoConfirm:
		case OM::Alert::Mode::ConfirmCancel:
			modalAlert.setOkBtnText(alert.mode == OM::Alert::Mode::InfoClose ? _("common.close") : _("common.ok"));
			modalAlert.okVisible(true);
			modalAlert.setOkCallback(
				[seq]()
				{
					LOG_INFO("MessageBox OK callback");
					Comm::DUET.SendGcodef("M292 S{:d}", seq);
				});
			break;

		case OM::Alert::Mode::Choices:
			modalAlert.setChoiceCallback(
				[seq](size_t index)
				{
					LOG_INFO("MessageBox Choice callback");
					Comm::DUET.SendGcodef("M292 R{{{:d}}} S{:d}", index, seq); // `{{` and `}}` to print `{` and `}`
				});
			break;

		case OM::Alert::Mode::NumberInt:
		{
			UI_LOCK();
			m_view->getKeyboard().setMode(LV_KEYBOARD_MODE_NUMBER);
			modalAlert.setKeyboard(&m_view->getKeyboard());

			if (alert.limits.numberInt.min > INT32_MIN)
			{
				modalAlert.setMinText(fmt::format("{:d}", alert.limits.numberInt.min));
				modalAlert.minTextVisible(true);
			}
			else
			{
				modalAlert.minTextVisible(false);
			}

			if (alert.limits.numberInt.max < INT32_MAX)
			{
				modalAlert.setMaxText(fmt::format("{:d}", alert.limits.numberInt.max));
				modalAlert.maxTextVisible(true);
			}
			else
			{
				modalAlert.maxTextVisible(false);
			}

			modalAlert.setInput(alert.limits.numberInt.valueDefault);

			// Store validation limits locally
			const int32_t min = alert.limits.numberInt.min;
			const int32_t max = alert.limits.numberInt.max;
			modalAlert.setInputValidationCallback(
				[min, max, &modalAlert](std::string_view text) -> bool
				{
					UI_LOCK();
					int value = std::atoi(text.data());
					bool valid = value >= min && value <= max;
					modalAlert.warningTextVisible(!valid);
					if (!valid)
					{
						modalAlert.setWarningText(_("msgbox.warning_int_range", min, max));
					}
					return valid;
				});

			modalAlert.setOkCallback(
				[seq, &modalAlert]()
				{
					UI_LOCK();
					int value = std::atoi(modalAlert.getInput().data());
					Comm::DUET.SendGcodef("M292 R{{{:d}}} S{:d}", value, seq);
				});
			modalAlert.setShowKeyboardCallback([this](bool show) { m_view->showKeyboard(show); });
		}
		break;

		case OM::Alert::Mode::NumberFloat:
			m_view->getKeyboard().setMode(LV_KEYBOARD_MODE_NUMBER);
			modalAlert.setKeyboard(&m_view->getKeyboard());

			if (alert.limits.numberFloat.min > -FLT_MAX)
			{
				modalAlert.setMinText(fmt::format("{:g}", alert.limits.numberFloat.min));
				modalAlert.minTextVisible(true);
			}
			else
			{
				modalAlert.minTextVisible(false);
			}

			if (alert.limits.numberFloat.max < FLT_MAX)
			{
				modalAlert.setMaxText(fmt::format("{:g}", alert.limits.numberFloat.max));
				modalAlert.maxTextVisible(true);
			}
			else
			{
				modalAlert.maxTextVisible(false);
			}

			modalAlert.setInput(alert.limits.numberFloat.valueDefault);
			modalAlert.setInputValidationCallback(
				[alert, &modalAlert](std::string_view text) -> bool
				{
					UI_LOCK();
					float value = static_cast<float>(std::atof(text.data()));
					bool valid = value >= alert.limits.numberFloat.min && value <= alert.limits.numberFloat.max;
					modalAlert.warningTextVisible(!valid);
					if (!valid)
					{
						modalAlert.setWarningText(_(
							"msgbox.warning_float_range", alert.limits.numberFloat.min, alert.limits.numberFloat.max));
					}
					return valid;
				});
			modalAlert.setOkCallback(
				[seq, &modalAlert]()
				{
					float value = static_cast<float>(std::atof(modalAlert.getInput().data()));
					Comm::DUET.SendGcodef("M292 R{{{:g}}} S{:d}", value, seq);
				});
			modalAlert.setShowKeyboardCallback([this](bool show) { m_view->showKeyboard(show); });
			break;
		case OM::Alert::Mode::Text:
		{
			m_view->getKeyboard().setMode(LV_KEYBOARD_MODE_TEXT_LOWER);
			modalAlert.setKeyboard(&m_view->getKeyboard());

			modalAlert.setInput(alert.limits.text.valueDefault.c_str());
			modalAlert.setInputValidationCallback(
				[alert, &modalAlert](std::string_view text) -> bool
				{
					UI_LOCK();
					int32_t len = (int32_t)text.length();
					bool valid = len >= alert.limits.text.min && len <= alert.limits.text.max;
					modalAlert.warningTextVisible(!valid);
					if (!valid)
					{
						modalAlert.setWarningText(
							_("msgbox.warning_text_length", alert.limits.text.min, alert.limits.text.max));
					}
					return valid;
				});
			modalAlert.setOkCallback(
				[seq, &modalAlert]()
				{
					std::string text(modalAlert.getInput());
					text = std::regex_replace(text, std::regex("\""), "\"\"");
					text = std::regex_replace(text, std::regex("\'"), "\'\'");
					Comm::DUET.SendGcodef("M292 R{{\"{:s}\"}} S{:d}", text, seq);
				});
			modalAlert.setShowKeyboardCallback([this](bool show) { m_view->showKeyboard(show); });
			break;
		}
		default:
			LOG_WARN("Unhandled alert mode {}", (int)alert.mode);
			break;
		}

		// Handle axis controls after mode setup
		if (alert.flags.IsBitSet(OM::Alert::GotControls))
		{
			size_t count = 0;
			LOG_DBG("axisControl {:d}", alert.controls);
			for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
			{
				if (!(alert.controls & (1 << i)))
					continue;
				auto axis = OM::Move::GetAxis(i);
				if (axis == nullptr)
					continue;
				m_alertAxes.push_back(axis->letter[0]);
				modalAlert.setJogAxisLetter(count, axis->letter[0]);
				modalAlert.setJogAxisPosition(count, axis->userPosition);
				modalAlert.setJogAxisEnabled(count, axis->homed);
				count++;
				LOG_DBG("Axis {:d}, count {:d}", i, count);
			}
			modalAlert.axisJogVisible(true);
		}

		// Set up cancel button if needed
		if (alert.cancelButton)
		{
			modalAlert.cancelVisible(true);
			modalAlert.setCancelCallback(
				[seq]()
				{
					LOG_INFO("MessageBox Cancel callback");
					Comm::DUET.SendGcodef("M292 P1 S{:d}", seq);
				});
		}

		// Configure timeout and progress last
		modalAlert.setTimeout(static_cast<uint32_t>(alert.timeout));
		modalAlert.progressVisible(alert.timeout > 0);
		if (alert.timeout > 0)
		{
			modalAlert.setProgressCallback([](MessageBox* msgBox) -> uint32_t
										   { return msgBox->getTimeOutPercentage(); });
		}

		// Finally show the message box
		modalAlert.open();
	}
} // namespace UI

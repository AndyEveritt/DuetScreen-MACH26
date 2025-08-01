#include "HomePresenter.h"
#include "HomeView.h"

#include "Hardware/Duet.h"
#include "Hardware/Usb.h"
#include "ObjectModel/Heat.h"
#include "ObjectModel/Sensor.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"
#include "utils/UpgradeHelper.h"
#include <algorithm>
#include <regex>

namespace UI
{
	void HomePresenter::onInit()
	{
		registerEventListener<EventType::Tick>(this, &HomePresenter::tick);
		registerEventListener<EventType::UpdateAvailable>(this, &HomePresenter::newUpdateAvailable);
		registerEventListener<EventType::AxesData>(this, &HomePresenter::newAxesData);
		registerEventListener<EventType::Response>(this, &HomePresenter::newResponse);
		registerEventListener<EventType::MessageBoxData>(this, &HomePresenter::newMessageBoxData);

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

	void HomePresenter::tick()
	{
		UI_LOCK();
		const size_t sensorCount = OM::GetAnalogSensorCount();
		m_view->m_graph.setSeriesCount(sensorCount);
		for (size_t i = 0; i < sensorCount; i++)
		{
			auto sensor = OM::GetAnalogSensorBySlot(i);
			if (!m_view->m_graph.getSeries(i))
			{
				m_view->m_graph.createSeries(lv_palette_main((lv_palette_t)m_view->m_graph.getSeriesCount()),
											 sensor->name.c_str());
			}
			m_view->m_graph.updateSeriesName(i, sensor->name.c_str());
			m_view->m_graph.addData(i, sensor->lastReading);
		}

		size_t heaterCount = OM::Heat::GetHeaterCount();
		if (heaterCount > 0)
		{
			int32_t maxTemperature = 300; // Default max temperature
			for (size_t i = 0; i < heaterCount; i++)
			{
				auto heater = OM::Heat::GetHeaterBySlot(i);
				if (heater == nullptr)
				{
					continue;
				}
				maxTemperature = std::max(maxTemperature, (int32_t)heater->max);
			}
			m_view->m_graph.setYRange({0, maxTemperature});
		}
	}

	void HomePresenter::clear()
	{
		UI_LOCK();
		m_view->clear();
		m_alertAxes.clear();
	}

	void HomePresenter::update()
	{
		UpgradeHelper::upgradeFromUSB(m_updateFile);
	}

	void HomePresenter::newUpdateAvailable(const std::string& file)
	{
		m_updateFile = file.c_str();
		m_view->showUpdatePrompt(true);
	}

	void HomePresenter::newAxesData()
	{
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

	void HomePresenter::newResponse(const std::string& resp)
	{
		UI_LOCK();
		if (m_view->m_consoleView.isVisible())
		{
			return;
		}

		if (resp.empty())
		{
			return;
		}

		std::shared_ptr<MessageBox> msgBox = m_view->createMessageBox();
		msgBox->setTitle("Response");
		msgBox->setText(resp);
		msgBox->setCancelCallback(
			[this]()
			{
				UI_LOCK();
				m_view->popMessageBox();
				if (m_view->getMessageBoxCount() > 0)
				{
					auto msgBox = m_view->getMessageBox(0);
					msgBox->show();
					msgBox->setTimeout(StorageHelper::getData(ID_INFO_TIMEOUT, DEFAULT_POPUP_TIMEOUT));
				}
			});

		msgBox->setCancelBtnText(_("msgbox_close"));
		msgBox->setOkBtnText(_("open_console"));
		msgBox->setOkCallback(
			[this]()
			{
				UI_LOCK();
				m_view->clearMessageBoxes();
				openScreen(&m_view->m_consoleView);
			});
		msgBox->okVisible(true);
		msgBox->progressVisible(true);
		if (m_view->getMessageBoxCount() == 1)
		{
			msgBox->show();
			msgBox->setTimeout(StorageHelper::getData(ID_INFO_TIMEOUT, DEFAULT_POPUP_TIMEOUT));
		}
		msgBox->setProgressCallback([](MessageBox* msgBox) -> uint32_t { return msgBox->getTimeOutPercentage(); });
		if (m_view->m_alert.isVisible())
		{
			msgBox->hide();
		}
	}

	void HomePresenter::newMessageBoxData(const OM::Alert& alert)
	{
		MessageBox& msgBox = m_view->m_alert;

		// First clear any existing alert state
		m_alertAxes.clear();

		if (alert.mode == OM::Alert::Mode::None)
		{
			if (msgBox.isVisible())
			{
				msgBox.close();
				msgBox.hide();
			}
			return;
		}

		// Configure the message box before showing it to prevent partial updates
		msgBox.clear();
		msgBox.setTitle(alert.title.c_str());
		msgBox.setText(alert.text.c_str());
		msgBox.setChoiceCount(alert.choices_count);

		for (size_t i = 0; i < alert.choices_count; i++)
		{
			msgBox.setChoice(i, alert.choices[i].c_str());
		}

		msgBox.setMode(alert.mode);

		// Set up close callback first to ensure proper cleanup
		msgBox.setCloseCallback(
			[this]()
			{
				UI_LOCK();
				m_view->m_alert.hide();
				if (m_view->getMessageBoxCount() > 0)
				{
					auto msgBox = m_view->getMessageBox(0);
					if (msgBox) // Add null check
					{
						msgBox->show();
						msgBox->setTimeout(StorageHelper::getData(ID_INFO_TIMEOUT, DEFAULT_POPUP_TIMEOUT));
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
			msgBox.setOkBtnText(alert.mode == OM::Alert::Mode::InfoClose ? _("msgbox_close") : _("msgbox_ok"));
			msgBox.okVisible(true);
			msgBox.setOkCallback(
				[seq]()
				{
					LOG_INFO("MessageBox OK callback");
					Comm::DUET.SendGcodef("M292 S%u", seq);
				});
			break;

		case OM::Alert::Mode::Choices:
			msgBox.setChoiceCallback(
				[seq](size_t index)
				{
					LOG_INFO("MessageBox Choice callback");
					Comm::DUET.SendGcodef("M292 R{%u} S%u", index, seq);
				});
			break;

		case OM::Alert::Mode::NumberInt:
		{
			UI_LOCK();
			lv_keyboard_set_mode(m_view->m_kb, LV_KEYBOARD_MODE_NUMBER);
			msgBox.setKeyboard(m_view->m_kb);

			if (alert.limits.numberInt.min > INT32_MIN)
			{
				msgBox.setMinTextf("%d", alert.limits.numberInt.min);
				msgBox.minTextVisible(true);
			}
			else
			{
				msgBox.minTextVisible(false);
			}

			if (alert.limits.numberInt.max < INT32_MAX)
			{
				msgBox.setMaxTextf("%d", alert.limits.numberInt.max);
				msgBox.maxTextVisible(true);
			}
			else
			{
				msgBox.maxTextVisible(false);
			}

			msgBox.setInput(alert.limits.numberInt.valueDefault);

			// Store validation limits locally
			const int32_t min = alert.limits.numberInt.min;
			const int32_t max = alert.limits.numberInt.max;
			msgBox.setInputValidationCallback(
				[min, max, &msgBox](const char* text) -> bool
				{
					UI_LOCK();
					int value = std::atoi(text);
					bool valid = value >= min && value <= max;
					msgBox.warningTextVisible(!valid);
					if (!valid)
					{
						msgBox.setWarningTextf(_("msgbox_warning_int_range"), min, max);
					}
					return valid;
				});

			msgBox.setOkCallback(
				[seq, &msgBox]()
				{
					UI_LOCK();
					int value = std::atoi(msgBox.getInput());
					Comm::DUET.SendGcodef("M292 R{%d} S%u", value, seq);
				});
			msgBox.setShowKeyboardCallback([this](bool show) { m_view->showKeyboard(show); });
		}
		break;

		case OM::Alert::Mode::NumberFloat:
			lv_keyboard_set_mode(m_view->m_kb, LV_KEYBOARD_MODE_NUMBER);
			msgBox.setKeyboard(m_view->m_kb);

			if (alert.limits.numberFloat.min > -FLT_MAX)
			{
				msgBox.setMinTextf("%.1f", alert.limits.numberFloat.min);
				msgBox.minTextVisible(true);
			}
			else
			{
				msgBox.minTextVisible(false);
			}

			if (alert.limits.numberFloat.max < FLT_MAX)
			{
				msgBox.setMaxTextf("%.1f", alert.limits.numberFloat.max);
				msgBox.maxTextVisible(true);
			}
			else
			{
				msgBox.maxTextVisible(false);
			}

			msgBox.setInput(alert.limits.numberFloat.valueDefault);
			msgBox.setInputValidationCallback(
				[alert, &msgBox](const char* text) -> bool
				{
					UI_LOCK();
					float value = std::atof(text);
					bool valid = value >= alert.limits.numberFloat.min && value <= alert.limits.numberFloat.max;
					msgBox.warningTextVisible(!valid);
					if (!valid)
					{
						msgBox.setWarningTextf(_("msgbox_warning_float_range"),
											   alert.limits.numberFloat.min,
											   alert.limits.numberFloat.max);
					}
					return valid;
				});
			msgBox.setOkCallback(
				[seq, &msgBox]()
				{
					float value = std::atof(msgBox.getInput());
					Comm::DUET.SendGcodef("M292 R{%.8f} S%u", value, seq);
				});
			msgBox.setShowKeyboardCallback([this](bool show) { m_view->showKeyboard(show); });
			break;
		case OM::Alert::Mode::Text:
		{
			lv_keyboard_set_mode(m_view->m_kb, LV_KEYBOARD_MODE_TEXT_LOWER);
			msgBox.setKeyboard(m_view->m_kb);

			msgBox.setInput(alert.limits.text.valueDefault.c_str());
			msgBox.setInputValidationCallback(
				[alert, &msgBox](const char* text) -> bool
				{
					UI_LOCK();
					int32_t len = (int32_t)strlen(text);
					bool valid = len >= alert.limits.text.min && len <= alert.limits.text.max;
					msgBox.warningTextVisible(!valid);
					if (!valid)
					{
						msgBox.setWarningTextf(
							_("msgbox_warning_text_length"), alert.limits.text.min, alert.limits.text.max);
					}
					return valid;
				});
			msgBox.setOkCallback(
				[seq, &msgBox]()
				{
					std::string text = msgBox.getInput();
					text = std::regex_replace(text, std::regex("\""), "\"\"");
					text = std::regex_replace(text, std::regex("\'"), "\'\'");
					Comm::DUET.SendGcodef("M292 R{\"%s\"} S%u", text.c_str(), seq);
				});
			msgBox.setShowKeyboardCallback([this](bool show) { m_view->showKeyboard(show); });
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
				msgBox.setJogAxisLetter(count, axis->letter[0]);
				count++;
				LOG_DBG("Axis {:d}, count {:d}", i, count);
			}
			msgBox.axisJogVisible(true);
		}

		// Set up cancel button if needed
		if (alert.cancelButton)
		{
			msgBox.cancelVisible(true);
			msgBox.setCancelCallback(
				[seq]()
				{
					LOG_INFO("MessageBox Cancel callback");
					Comm::DUET.SendGcodef("M292 P1 S%u", seq);
				});
		}

		// Configure timeout and progress last
		msgBox.setTimeout(alert.timeout);
		msgBox.progressVisible(alert.timeout > 0);
		if (alert.timeout > 0)
		{
			msgBox.setProgressCallback([](MessageBox* msgBox) -> uint32_t { return msgBox->getTimeOutPercentage(); });
		}

		// Finally show the message box
		msgBox.show();
	}
} // namespace UI

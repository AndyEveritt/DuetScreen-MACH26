#include "StatusBar.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "i18n/i18n.h"

namespace UI
{
	StatusBar::StatusBar(LvObj& parent)
		: View("status_bar", parent)
	{
		ZoneScoped;
		activate();

		UI_LOCK();

		setSize(LV_PCT(100), LV_SIZE_CONTENT);
		setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		m_duetName.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);
		m_temperatures.setStyleTextAlign(LV_TEXT_ALIGN_RIGHT);

		m_duetStatus.setAlign(LV_ALIGN_LEFT_MID, 0, 0);
		m_duetName.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_time.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);

		m_temperatureContainer.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_temperatures.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_temperatureContainer.setStylePad(0);

		m_temperatureContainer.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_temperatures.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		addEventCallback([this](lv_event_t*) { updateTemperaturesPosition(); },
						 static_cast<lv_event_code_t>(LV_EVENT_SIZE_CHANGED | LV_EVENT_STYLE_CHANGED));
		m_duetName.addEventCallback([this](lv_event_t*) { updateTemperaturesPosition(); },
									static_cast<lv_event_code_t>(LV_EVENT_SIZE_CHANGED | LV_EVENT_STYLE_CHANGED));

		m_duetName.setMaxWidth(LV_PCT(40));

		m_duetName.setLongMode(LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
		m_temperatures.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		m_time.hide();
	}

	void StatusBar::setDuetName(std::string_view name)
	{
		ZoneScoped;
		UI_LOCK();
		m_duetName.setText(name);
		updateTemperaturesPosition();
	}

	void StatusBar::setDuetStatus(std::string_view status)
	{
		ZoneScoped;
		UI_LOCK();
		m_duetStatus.setText(status);
	}

	void StatusBar::setTime(std::string_view time)
	{
		ZoneScoped;
		UI_LOCK();
		m_time.setText(time);
	}

	void StatusBar::updateTemperaturesPosition()
	{
		ZoneScoped;
		UI_LOCK();
		updateLayout();
		constexpr lv_coord_t pad = 10;
		m_temperatureContainer.setWidth(m_temperatureContainer.getCoords().x2 - m_duetName.getCoords().x2 - pad);
	}
} // namespace UI

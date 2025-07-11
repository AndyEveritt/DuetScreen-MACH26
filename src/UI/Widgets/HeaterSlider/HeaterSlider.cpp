#include "HeaterSlider.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include <math.h>

namespace UI
{
	HeaterSlider::HeaterSlider(const std::string& name, lv_obj_t* parent)
		: View(lv_obj_create, name, parent)
		, m_heaterInfoCont(name + "_heater_info_cont", getRoot())
		, m_heaterName(name + "_heater_name", m_heaterInfoCont)
		, m_heaterState(name + "_heater_state", m_heaterInfoCont)
		, m_temperatureCont(name + "_temperature_cont", getRoot())
		, m_currentTemperature(name + "_current_temperature", m_temperatureCont)
		, m_activeTemperature(name + "_active_temperature", m_temperatureCont)
		, m_standbyTemperature(name + "_standby_temperature", m_temperatureCont)
	{
		UI_LOCK();

		// Set up the heater slider view
		m_heaterName.setText("Heater");
		m_heaterState.setText("State");
		m_activeTemperature.setText("Active Temp");
		m_standbyTemperature.setText("Standby Temp");

		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_heaterInfoCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_heaterInfoCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		m_heaterInfoCont.setSize(LV_SIZE_CONTENT, LV_PCT(100));
		m_heaterInfoCont.setMinHeight(LV_SIZE_CONTENT);
		m_temperatureCont.setHeight(LV_PCT(100));
		m_temperatureCont.setMinHeight(100);
		m_temperatureCont.setFlexGrow(1);

		m_currentTemperature.setSize(LV_PCT(100), 15);
		// lv_coord_t
		m_currentTemperature.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_activeTemperature.setAlign(LV_ALIGN_LEFT_MID, 0, -25);
		m_standbyTemperature.setAlign(LV_ALIGN_LEFT_MID, 0, 25);
		// m_activeTemperature.setFlag(LV_OBJ_FLAG_FLOATING, true);
		// m_standbyTemperature.setFlag(LV_OBJ_FLAG_FLOATING, true);

		m_activeTemperature.setUserData(this);
		m_standbyTemperature.setUserData(this);
		m_activeTemperature.addEventCallback(onActiveTemperatureEvent, LV_EVENT_ALL, &m_activeTemperature);
		m_standbyTemperature.addEventCallback(onActiveTemperatureEvent, LV_EVENT_ALL, &m_standbyTemperature);
		m_currentTemperature.addEventCallback(drawCurrentTemperatureEvent, LV_EVENT_DRAW_MAIN_END, this);

		// Add styles
		m_heaterInfoCont.addStyle(Themes::getLvglStyles().no_border);
		m_temperatureCont.addStyle(Themes::getLvglStyles().no_border);
		m_activeTemperature.addStyle(Themes::getLvglStyles().pad_normal);
		m_standbyTemperature.addStyle(Themes::getLvglStyles().pad_normal);
		m_activeTemperature.addStyle(Themes::getLvglStyles().input);
		m_standbyTemperature.addStyle(Themes::getLvglStyles().input);

		m_currentTemperature.addStyle(Themes::getComponentStyles().temperature_bar, LV_PART_INDICATOR);
	}

	void HeaterSlider::setHeaterName(const std::string& name)
	{
		m_heaterName.setText(name);
	}

	void HeaterSlider::setHeaterState(const std::string& state)
	{
		m_heaterState.setText(state);
	}

	void HeaterSlider::setHeaterMinTemperature(float temperature)
	{
		UI_LOCK();
		m_minTempValue = std::max(0.0f, temperature);
		m_currentTemperature.setMinValue(m_minTempValue);
		updateLabelPositions();
	}

	void HeaterSlider::setHeaterMaxTemperature(float temperature)
	{
		UI_LOCK();
		m_maxTempValue = temperature;
		m_currentTemperature.setMaxValue(temperature);
		updateLabelPositions();
	}

	void HeaterSlider::setCurrentTemperature(float temperature)
	{
		UI_LOCK();
		m_currentTempValue = temperature;
		m_currentTemperature.setValue(temperature);
	}

	void HeaterSlider::setActiveTemperature(float temperature)
	{
		m_activeTempValue = temperature;
		m_activeTemperature.setText(fmt::format("{:g} °C", temperature));
		updateLabelPositions();
	}

	void HeaterSlider::setStandbyTemperature(float temperature)
	{
		m_standbyTempValue = temperature;
		m_standbyTemperature.setText(fmt::format("{:g} °C", temperature));
		updateLabelPositions();
	}

	void HeaterSlider::drawCurrentTemperatureEvent(lv_event_t* e)
	{
		HeaterSlider& slider = *(HeaterSlider*)lv_event_get_user_data(e);

		lv_draw_label_dsc_t label_dsc;
		lv_draw_label_dsc_init(&label_dsc);
		label_dsc.font = LV_FONT_DEFAULT;

		char buf[8];
		snprintf(buf, sizeof(buf), "%.1f", slider.m_currentTempValue);

		lv_point_t txt_size;
		lv_text_get_size(
			&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

		lv_area_t txt_area;
		txt_area.x1 = 0;
		txt_area.x2 = txt_size.x - 1;
		txt_area.y1 = 0;
		txt_area.y2 = txt_size.y - 1;

		lv_area_t indic_area;
		lv_obj_get_coords(slider.m_currentTemperature, &indic_area);
		lv_area_set_width(&indic_area,
						  lv_area_get_width(&indic_area) * slider.m_currentTempValue /
							  (slider.m_maxTempValue - slider.m_minTempValue));

		/*If the indicator is long enough put the text inside on the right*/
		if (lv_area_get_width(&indic_area) > txt_size.x + 20)
		{
			lv_area_align(&indic_area, &txt_area, LV_ALIGN_RIGHT_MID, -10, 0);
			label_dsc.color = lv_color_white();
		}
		/*If the indicator is still short put the text out of it on the right*/
		else
		{
			lv_area_align(&indic_area, &txt_area, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
			label_dsc.color = lv_color_white();
		}
		label_dsc.text = buf;
		label_dsc.text_local = true;
		lv_layer_t* layer = lv_event_get_layer(e);
		lv_draw_label(layer, &label_dsc, &txt_area);
	}

	void HeaterSlider::onActiveTemperatureEvent(lv_event_t* e)
	{
		LvLabel& label = *(LvLabel*)lv_event_get_user_data(e);
		HeaterSlider& control = *(HeaterSlider*)label.getUserData();
		lv_event_code_t code = lv_event_get_code(e);

		bool activeTemperature;
		if (label == control.m_activeTemperature)
		{
			activeTemperature = true;
		}
		else if (label == control.m_standbyTemperature)
		{
			activeTemperature = false;
		}
		else
		{
			LOG_ERROR("Unexpected label in HeaterSlider event handler");
			return; // Not a temperature label
		}
		float temperature = activeTemperature ? control.m_activeTempValue : control.m_standbyTempValue;

		switch (code)
		{
		case LV_EVENT_CLICKED:
		{
			// Open numberpad
			break;
		}
		case LV_EVENT_PRESSING:
		{
			// Update the target temperature based on the slider position
			break;
		}
		case LV_EVENT_RELEASED:
		case LV_EVENT_PRESS_LOST:
		{
			// Set new target temperature
			break;
		}
		case LV_EVENT_REFR_EXT_DRAW_SIZE:
		{
			int32_t* size = static_cast<int32_t*>(lv_event_get_param(e));
			*size = std::max(*size, 1000); // Ensure enough space for the label
			break;
		}
		case LV_EVENT_DRAW_MAIN:
		{
			lv_layer_t* layer = lv_event_get_layer(e);
			lv_area_t marker_area;
			static int32_t marker_width = 5;

			lv_draw_rect_dsc_t marker_dsc;
			lv_draw_rect_dsc_init(&marker_dsc);
			marker_dsc.base.layer = layer;
			lv_obj_init_draw_rect_dsc(control, LV_PART_INDICATOR, &marker_dsc);
			marker_dsc.bg_color = lv_palette_main(LV_PALETTE_RED);
			marker_dsc.bg_opa = LV_OPA_COVER;

			lv_area_t label_area = label.getCoords();
			lv_coord_t label_width = label.getWidth();

			float pct = std::clamp(
				(temperature - control.m_minTempValue) / (control.m_maxTempValue - control.m_minTempValue), 0.0f, 1.0f);

			marker_area.x1 = label_area.x1 + label_width * pct - marker_width / 2;
			marker_area.x2 = marker_area.x1 + marker_width - 1;

			if (label == control.m_activeTemperature)
			{
				marker_area.y1 = label_area.y2 + 1;
				marker_area.y2 = control.m_currentTemperature.getCoords().y1;
			}
			else if (label == control.m_standbyTemperature)
			{
				marker_area.y1 = control.m_currentTemperature.getCoords().y2 + 1;
				marker_area.y2 = label_area.y1 - 1;
			}

			lv_draw_rect(layer, &marker_dsc, &marker_area);
			break;
		}
		}
	}

	void HeaterSlider::updateLabelPositions()
	{
		updateLabelPosition(m_activeTemperature, m_activeTempValue);
		updateLabelPosition(m_standbyTemperature, m_standbyTempValue);
	}

	void HeaterSlider::updateLabelPosition(LvLabel& label, float value)
	{
		// Calculate the position based on the current temperature value
		lv_coord_t percentage =
			100 * std::clamp((value - m_minTempValue) / (m_maxTempValue - m_minTempValue), 0.0f, 1.0f);

		label.updateLayout();
		lv_coord_t label_width = label.getWidth();
		lv_coord_t bar_width = m_currentTemperature.getWidth();

		lv_coord_t label_offset_pct = percentage * label_width / bar_width;

		label.setX(LV_PCT(percentage - label_offset_pct));
	}
} // namespace UI

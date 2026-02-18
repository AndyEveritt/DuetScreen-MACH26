#pragma once

#include "HeaterSliderPresenter.h"
#include "UI/Components/Bar/Bar.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Core/View.h"

namespace UI
{
	class HeaterSlider : public View<HeaterSliderPresenter>
	{
	  public:
		HeaterSlider(const std::string& name, LvObj& parent);

		void setHeaterName(const std::string_view name);
		void setHeaterState(HeaterSliderPresenter::heater_state_t state, const std::string_view str);
		void setHeaterMinTemperature(float temperature);
		void setHeaterMaxTemperature(float temperature);
		void setCurrentTemperature(float temperature);
		void setActiveTemperature(int32_t temperature, bool dragging = false);
		void setStandbyTemperature(int32_t temperature, bool dragging = false);
		void setDisabled(bool disabled);

		void setNumberPad(ModalNumberPad* numberPad);

	  private:
		static void onTemperatureLabelEvent(lv_event_t* e);
		static void onToggleStateEvent(lv_event_t* e);

		void onShow() override;

		void updateLabelPositions();
		void updateLabelPosition(LvObj& label, int32_t value);

		LvContainer m_heaterInfoCont{"heater_info_cont", getRoot()};
		Button m_heaterName{"heater_name", m_heaterInfoCont};
		LvLabel m_heaterState{"heater_state", m_heaterInfoCont};
		LvContainer m_temperatureCont{"temperature_cont", getRoot()};
		Bar m_currentTemperature{"current_temperature", m_temperatureCont};
		Button m_activeTemperature{"active_temperature", m_temperatureCont};
		Button m_standbyTemperature{"standby_temperature", m_temperatureCont};

		lv_point_t m_pressedPoint;
		lv_point_t m_pressedPointOffset;
		bool m_dragging;

		float m_currentTempValue = 0.0f;
		int32_t m_activeTempValue = 0;
		int32_t m_standbyTempValue = 0;
		float m_minTempValue = 0.0f;
		float m_maxTempValue = 0.0f;

		ModalNumberPad* m_numberPad = nullptr;
	};
} // namespace UI
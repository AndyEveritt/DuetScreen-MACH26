#pragma once

#include "HeaterSliderPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvBar.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Core/View.h"

namespace UI
{
	class HeaterSlider : public View<HeaterSliderPresenter>
	{
	  public:
		HeaterSlider(const std::string& name, lv_obj_t* parent);

		void setHeaterName(const std::string& name);
		void setHeaterState(const std::string& state);
		void setHeaterMinTemperature(float temperature);
		void setHeaterMaxTemperature(float temperature);
		void setCurrentTemperature(float temperature);
		void setActiveTemperature(float temperature, bool dragging = false);
		void setStandbyTemperature(float temperature, bool dragging = false);

	  private:
		static void onTemperatureLabelEvent(lv_event_t* e);
		static void drawCurrentTemperatureEvent(lv_event_t* e);

		void updateLabelPositions();
		void updateLabelPosition(LvLabel& label, float value);

		LvContainer m_heaterInfoCont;
		Button m_heaterName;
		LvLabel m_heaterState;
		LvContainer m_temperatureCont;
		LvBar m_currentTemperature;
		LvLabel m_activeTemperature;
		LvLabel m_standbyTemperature;

		lv_point_t m_pressedPoint;

		float m_currentTempValue = 0.0f;
		float m_activeTempValue = 0.0f;
		float m_standbyTempValue = 0.0f;
		float m_minTempValue = 0.0f;
		float m_maxTempValue = 300.0f;
	};
} // namespace UI
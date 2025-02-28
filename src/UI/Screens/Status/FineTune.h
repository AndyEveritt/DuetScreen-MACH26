/*
 * FineTune.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "FineTunePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/Slider.h"
#include "UI/Core/View.h"

namespace UI
{
	class FineTune : public View<FineTunePresenter>
	{
	  public:
		FineTune(lv_obj_t* parent);

		void setBabyStepValue(float value);

		void setSpeedValue(uint32_t value);

		void setExtruderCount(size_t count);
		void setFanCount(size_t count);
		size_t getExtruderCount() const { return m_extruders.size(); }
		size_t getFanCount() const { return m_fans.size(); }

		void setExtruderLabel(size_t index, const char* label);
		void setExtruderValue(size_t index, uint32_t value);

		void setFanLabel(size_t index, const char* label);
		void setFanValue(size_t index, uint32_t value);

	  protected:
		static void onBabyStepIncrementClicked(lv_event_t* e);
		static void onBabyStepDecrementClicked(lv_event_t* e);
		static void onBabyStepResetClicked(lv_event_t* e);
		static void onBabyStepValueClicked(lv_event_t* e);

		void showKeyboard(bool show);

		class Item : public BaseView
		{
		  public:
			Item(lv_obj_t* parent);
			void setIncrementLabel(const char* label);
			void setDecrementLabel(const char* label);
			void setResetLabel(const char* label);
			void setValueLabels(const std::array<const char*, 2>& labels);

		  private:
			Button m_reset;
			Button m_increment;
			Button m_decrement;

			lv_obj_t* m_valueCont;
			Button m_values[2];
		};

		Item m_babystep;
		lv_obj_t* m_sliderCont;

		// Speed Factor
		Slider m_speed;

		// Extruders
		lv_obj_t* m_extruderLabel;
		lv_obj_t* m_extruderCont;
		std::vector<std::shared_ptr<Slider>> m_extruders;

		// Fans
		lv_obj_t* m_fanLabel;
		lv_obj_t* m_fanCont;
		std::vector<std::shared_ptr<Slider>> m_fans;

		lv_obj_t* m_keyboard;
	};
} // namespace UI

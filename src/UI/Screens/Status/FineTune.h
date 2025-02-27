/*
 * FineTune.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "FineTunePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Core/View.h"

namespace UI
{
	class FineTune : public View<FineTunePresenter>
	{
	  public:
		FineTune(lv_obj_t* parent);

		void setBabyStepValue(float value);
		void setSpeedValue(float value);
		void setFlowValue(float value);

	  protected:
		static void onBabyStepIncrementClicked(lv_event_t* e);
		static void onBabyStepDecrementClicked(lv_event_t* e);
		static void onBabyStepResetClicked(lv_event_t* e);
		static void onBabyStepValueClicked(lv_event_t* e);

		static void onSpeedIncrementClicked(lv_event_t* e);
		static void onSpeedDecrementClicked(lv_event_t* e);
		static void onSpeedResetClicked(lv_event_t* e);
		static void onSpeedValueClicked(lv_event_t* e);

		static void onFlowIncrementClicked(lv_event_t* e);
		static void onFlowDecrementClicked(lv_event_t* e);
		static void onFlowResetClicked(lv_event_t* e);
		static void onFlowValueClicked(lv_event_t* e);

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
		Item m_speed;
		Item m_flow;
	};
} // namespace UI

/*
 * GenericAxisControl.h
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/Label.h"
#include "UI/Core/View.h"

namespace UI
{
	class GenericAxisControl : public LvObj
	{
		using position_cb_t = std::function<void(bool, void*)>;
		using home_cb_t = std::function<void(void*)>;

	  public:
		GenericAxisControl(const std::string& name, lv_obj_t* parent, layout_t layout);

		void setAxisLetter(std::string_view letter);
		void setAxisPosition(float value);

		void setPositionCallback(position_cb_t cb, void* user_data);
		void setHomeCallback(home_cb_t cb, void* user_data);

	  private:
		static void onIncrementBtn(lv_event_t* event);
		static void onDecrementBtn(lv_event_t* event);
		static void onHomeBtn(lv_event_t* event);

		void updateLabel();

		Label m_label;
		Button m_incrementButton;
		Button m_homeButton;
		Button m_decrementButton;

		std::string m_axisLetter;
		float m_axisPosition = 0.0f;

		position_cb_t m_positionCallback;
		void* m_positionUserData = nullptr;

		home_cb_t m_homeCallback;
		void* m_homeUserData = nullptr;
	};
} // namespace UI

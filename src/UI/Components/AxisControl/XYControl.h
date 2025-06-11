/*
 * XYControl.h
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
	class XYControl : public LvObj
	{
		using position_cb_t = std::function<void(bool, void*)>;
		using home_cb_t = std::function<void(void*)>;

	  public:
		XYControl(const std::string& name, lv_obj_t* parent, layout_t layout);

		void setXPosition(float position);
		void setYPosition(float position);
		void setXPositionCallback(position_cb_t cb, void* user_data);
		void setYPositionCallback(position_cb_t cb, void* user_data);
		void setHomeXYCallback(home_cb_t cb, void* user_data);
		void setHomeXCallback(home_cb_t cb, void* user_data);
		void setHomeYCallback(home_cb_t cb, void* user_data);

	  private:
		static void onIncrementBtn(lv_event_t* event);
		static void onDecrementBtn(lv_event_t* event);
		static void onHomeXYBtn(lv_event_t* event);
		static void onHomeXBtn(lv_event_t* event);
		static void onHomeYBtn(lv_event_t* event);

		int32_t m_colDsc[5] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
		int32_t m_rowDsc[5] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

		Label m_xLabel;
		Label m_yLabel;
		Button m_xIncrementButton;
		Button m_xDecrementButton;
		Button m_yIncrementButton;
		Button m_yDecrementButton;
		Button m_homeXYButton;
		Button m_homeXButton;
		Button m_homeYButton;

		position_cb_t m_xPositionCallback;
		position_cb_t m_yPositionCallback;
		void* m_xPositionUserData = nullptr;
		void* m_yPositionUserData = nullptr;

		home_cb_t m_homeXYCallback;
		home_cb_t m_homeXCallback;
		home_cb_t m_homeYCallback;
		void* m_homeXYUserData = nullptr;
		void* m_homeXUserData = nullptr;
		void* m_homeYUserData = nullptr;
	};
} // namespace UI

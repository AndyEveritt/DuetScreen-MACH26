/*
 * LvSlider.cpp
 *
 *  Created on: 2025-07-10
 *      Author: Andy Everitt
 */

#include "LvSlider.h"
#include "Debug.h"

namespace UI
{
	LvSlider::LvSlider(const std::string& name, LvObj& parent)
		: LvObj(lv_slider_create, name, parent)
	{
		UI_LOCK();
	}

	void LvSlider::setValue(int32_t value, lv_anim_enable_t anim)
	{
		UI_LOCK();
		lv_slider_set_value(getRootPtr(), value, anim);
	}

	void LvSlider::setStartValue(int32_t value, lv_anim_enable_t anim)
	{
		UI_LOCK();
		lv_slider_set_start_value(getRootPtr(), value, anim);
	}

	void LvSlider::setRange(int32_t min, int32_t max)
	{
		UI_LOCK();
		lv_slider_set_range(getRootPtr(), min, max);
	}

	void LvSlider::setMinValue(int32_t min)
	{
		UI_LOCK();
		lv_slider_set_min_value(getRootPtr(), min);
	}

	void LvSlider::setMaxValue(int32_t max)
	{
		UI_LOCK();
		lv_slider_set_max_value(getRootPtr(), max);
	}

	void LvSlider::setMode(lv_slider_mode_t mode)
	{
		UI_LOCK();
		lv_slider_set_mode(getRootPtr(), mode);
	}

	void LvSlider::setOrientation(lv_slider_orientation_t orientation)
	{
		UI_LOCK();
		lv_slider_set_orientation(getRootPtr(), orientation);
	}

	int32_t LvSlider::getValue() const
	{
		UI_LOCK();
		return lv_slider_get_value(getRootPtr());
	}

	int32_t LvSlider::getLeftValue() const
	{
		UI_LOCK();
		return lv_slider_get_left_value(getRootPtr());
	}

	int32_t LvSlider::getMinValue() const
	{
		UI_LOCK();
		return lv_slider_get_min_value(getRootPtr());
	}

	int32_t LvSlider::getMaxValue() const
	{
		UI_LOCK();
		return lv_slider_get_max_value(getRootPtr());
	}

	bool LvSlider::isDragged() const
	{
		UI_LOCK();
		return lv_slider_is_dragged(getRootPtr());
	}

	lv_slider_mode_t LvSlider::getMode() const
	{
		UI_LOCK();
		return lv_slider_get_mode(getRootPtr());
	}

	lv_slider_orientation_t LvSlider::getOrientation() const
	{
		UI_LOCK();
		return lv_slider_get_orientation(getRootPtr());
	}

	bool LvSlider::isSymmetrical() const
	{
		UI_LOCK();
		return lv_slider_is_symmetrical(getRootPtr());
	}
} // namespace UI

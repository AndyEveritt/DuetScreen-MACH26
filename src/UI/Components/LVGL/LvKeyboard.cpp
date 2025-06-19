/*
 * LvKeyboard.cpp
 *
 *  Created on: 2025-06-16
 *      Author: Andy Everitt
 */

#include "LvKeyboard.h"
#include "Debug.h"

namespace UI
{
	LvKeyboard::LvKeyboard(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_keyboard_create, name, parent)
	{
		UI_LOCK();
		setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	}

	void LvKeyboard::setTextArea(LvTextArea* textArea)
	{
		UI_LOCK();
		lv_keyboard_set_textarea(getCont(), textArea ? textArea->getTextArea() : nullptr);
	}

	void LvKeyboard::setMode(lv_keyboard_mode_t mode)
	{
		UI_LOCK();
		lv_keyboard_set_mode(getCont(), mode);
	}

	void LvKeyboard::setPopovers(bool enable)
	{
		UI_LOCK();
		lv_keyboard_set_popovers(getCont(), enable);
	}

	void LvKeyboard::setMap(lv_keyboard_mode_t mode, const char* map[], const lv_buttonmatrix_ctrl_t ctrl_map[])
	{
		UI_LOCK();
		lv_keyboard_set_map(getCont(), mode, map, ctrl_map);
	}

	lv_obj_t* LvKeyboard::getTextArea() const
	{
		UI_LOCK();
		return lv_keyboard_get_textarea(getCont());
	}

	lv_keyboard_mode_t LvKeyboard::getMode() const
	{
		UI_LOCK();
		return lv_keyboard_get_mode(getCont());
	}

	bool LvKeyboard::getPopovers() const
	{
		UI_LOCK();
		return lv_keyboard_get_popovers(getCont());
	}

	const char* const* LvKeyboard::getMapArray() const
	{
		UI_LOCK();
		return lv_keyboard_get_map_array(getCont());
	}

	uint32_t LvKeyboard::getSelectedButton() const
	{
		UI_LOCK();
		return lv_buttonmatrix_get_selected_button(getCont());
	}

	const char* LvKeyboard::getButtonText(uint32_t index) const
	{
		UI_LOCK();
		return lv_buttonmatrix_get_button_text(getCont(), index);
	}
} // namespace UI

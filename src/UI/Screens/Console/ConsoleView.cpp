#include "ConsoleView.h"
#include "Debug.h"
#include "Gcodes.h"
#include "Hardware/Duet.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	ConsoleView::ConsoleView(lv_obj_t* parent)
		: View("console_view", parent, layout_t(0, 0, 100, 100))
		, m_topCont(lv_obj_create(getCont()))
		, m_commandList(lv_table_create(m_topCont))
		, m_output(lv_textarea_create(m_topCont))
		, m_inputCont(lv_obj_create(getCont()))
		, m_input(lv_textarea_create(m_inputCont))
		, m_clear("console_clear", m_input, LV_SYMBOL_CLOSE)
		, m_enter("console_enter", m_inputCont, LV_SYMBOL_NEW_LINE)
		, m_kb(lv_keyboard_create(getCont()))
	{
		Lock lock;

		// Layout
		lv_obj_align(getCont(), LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_width(m_topCont, LV_PCT(100));
		lv_obj_set_flex_grow(m_topCont, 1);
		lv_obj_set_size(m_inputCont, LV_PCT(100), LV_SIZE_CONTENT);

		// Top Container
		lv_obj_set_flex_flow(m_topCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_grow(m_commandList, 2);
		lv_obj_set_flex_grow(m_output, 3);
		lv_obj_set_height(m_commandList, LV_PCT(100));
		lv_obj_set_height(m_output, LV_PCT(100));
		lv_textarea_set_cursor_click_pos(m_output, false);

		// Command List
		lv_table_set_column_count(m_commandList, 2);
		lv_table_set_column_width(m_commandList, 0, 100);
		lv_table_set_column_width(m_commandList, 1, 500);
		lv_table_set_row_count(m_commandList, Gcodes::getGcodeCount());
		for (size_t i = 0; i < Gcodes::getGcodeCount(); i++)
		{
			const gcode* g = Gcodes::getGcode(i);
			lv_table_set_cell_value(m_commandList, i, 0, g->gcode);
			lv_table_set_cell_value(m_commandList, i, 1, g->helpText);
		}

		// Input Area
		lv_obj_set_flex_align(m_inputCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_flow(m_inputCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_style_pad_all(m_inputCont, 0, 0);
		lv_obj_set_flex_grow(m_input, 1);
		lv_textarea_set_one_line(m_input, true);
		lv_textarea_set_placeholder_text(m_input, _("console_input_placeholder"));
		lv_obj_align(m_clear.getCont(), LV_ALIGN_RIGHT_MID, 0, 0);
		lv_obj_set_height(m_input, LV_SIZE_CONTENT);
		lv_obj_set_size(m_clear.getCont(), LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_size(m_enter.getCont(), LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(m_input, 0, 0);
		lv_obj_set_style_pad_all(m_clear.getCont(), 0, 0);
		lv_obj_set_style_pad_all(m_enter.getCont(), 0, 0);

		// Hide keyboard initially
		lv_keyboard_set_mode(m_kb, LV_KEYBOARD_MODE_TEXT_LOWER);
		lv_obj_add_flag(m_kb, LV_OBJ_FLAG_HIDDEN);
	}

	void ConsoleView::clear()
	{
		Lock lock;
		lv_textarea_set_text(m_output, "");
	}

	void ConsoleView::addResponse(const char* resp)
	{
		Lock lock;

		lv_textarea_add_text(m_output, resp);
		lv_textarea_add_char(m_output, '\n');

		std::string currentText = lv_textarea_get_text(m_output);

		int newLineCount = std::count(currentText.begin(), currentText.end(), '\n');

		if (newLineCount > MAX_RESPONSE_LINES)
		{
			size_t pos = 0;
			for (int i = 0; i < newLineCount - MAX_RESPONSE_LINES; ++i)
			{
				pos = currentText.find('\n', pos) + 1;
			}
			currentText = currentText.substr(pos);
		}

		lv_textarea_set_text(m_output, currentText.c_str());
	}

	bool ConsoleView::back()
	{
		// TODO close keyboard if open
		return false;
	}

	void ConsoleView::onShow()
	{
		lv_obj_scroll_to_x(m_commandList, 0, LV_ANIM_OFF);
		lv_obj_add_flag(m_kb, LV_OBJ_FLAG_HIDDEN);
	}

	void ConsoleView::onHide()
	{
		activate();
	}
} // namespace UI

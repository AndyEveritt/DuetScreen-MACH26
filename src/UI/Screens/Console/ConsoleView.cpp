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
		UI_LOCK();

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
		lv_obj_set_style_text_align(m_input, LV_TEXT_ALIGN_LEFT, 0);
		lv_obj_align(m_clear.getCont(), LV_ALIGN_RIGHT_MID, 0, 0);
		lv_obj_set_height(m_input, LV_SIZE_CONTENT);
		lv_obj_set_size(m_clear.getCont(), LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_size(m_enter.getCont(), LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(m_input, 0, 0);
		lv_obj_set_style_pad_all(m_clear.getCont(), 0, 0);
		lv_obj_set_style_pad_all(m_enter.getCont(), 0, 0);

		// Hide keyboard initially
		lv_keyboard_set_mode(m_kb, LV_KEYBOARD_MODE_TEXT_UPPER);
		lv_obj_add_flag(m_kb, LV_OBJ_FLAG_HIDDEN);

		// Callbacks
		m_clear.setCallback(onClearEvent, LV_EVENT_CLICKED, this);
		m_enter.setCallback(onSendEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_commandList, onCommandListEvent, LV_EVENT_ALL, this);
		lv_obj_add_event_cb(m_input, onKeyboardEvent, LV_EVENT_ALL, this);
	}

	void ConsoleView::clear()
	{
		UI_LOCK();
		lv_textarea_set_text(m_input, "");
	}

	void ConsoleView::addCommand(const char* resp)
	{
		addResponse(utils::format("> %s", resp).c_str());
	}

	void ConsoleView::addResponse(const char* resp)
	{
		UI_LOCK();

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
			lv_textarea_set_text(m_output, currentText.c_str());
		}
	}

	void ConsoleView::onSendEvent(lv_event_t* e)
	{
		UI_LOCK();
		ConsoleView* view = static_cast<ConsoleView*>(lv_event_get_user_data(e));
		lv_obj_send_event(view->m_input, LV_EVENT_READY, view);
	}

	void ConsoleView::onClearEvent(lv_event_t* e)
	{
		UI_LOCK();
		ConsoleView* view = static_cast<ConsoleView*>(lv_event_get_user_data(e));
		view->clear();
	}

	void ConsoleView::onCommandListEvent(lv_event_t* e)
	{
		UI_LOCK();
		ConsoleView* view = static_cast<ConsoleView*>(lv_event_get_user_data(e));
		lv_event_code_t code = lv_event_get_code(e);
		if (code == LV_EVENT_VALUE_CHANGED)
		{
			uint32_t row;
			uint32_t col;
			lv_table_get_selected_cell(view->m_commandList, &row, &col);

			const char* gcode = lv_table_get_cell_value(view->m_commandList, row, 0);

			if (gcode[0] == '\0')
			{
				return;
			}
			lv_textarea_set_text(view->m_input, gcode);
		}
	}

	void ConsoleView::onKeyboardEvent(lv_event_t* e)
	{
		UI_LOCK();
		ConsoleView* view = static_cast<ConsoleView*>(lv_event_get_user_data(e));
		lv_event_code_t code = lv_event_get_code(e);
		switch (code)
		{
		case LV_EVENT_FOCUSED:
		{
			lv_keyboard_set_textarea(view->m_kb, view->m_input);
			lv_keyboard_set_mode(view->m_kb, LV_KEYBOARD_MODE_TEXT_UPPER);
			lv_obj_remove_flag(view->m_kb, LV_OBJ_FLAG_HIDDEN);
			break;
		}
		case LV_EVENT_DEFOCUSED:
		{
			lv_keyboard_set_textarea(view->m_kb, NULL);
			lv_obj_add_flag(view->m_kb, LV_OBJ_FLAG_HIDDEN);
			break;
		}
		case LV_EVENT_VALUE_CHANGED:
		{
			lv_obj_scroll_to_y(view->m_commandList, 0, LV_ANIM_OFF);
			std::string cmd = lv_textarea_get_text(view->m_input);
			if (cmd.find_first_of(' ') == std::string::npos)
			{
				std::string upper_cmd;
				std::transform(cmd.begin(),
							   cmd.end(),
							   std::back_inserter(upper_cmd),
							   [](unsigned char c) { return std::toupper(c); });

				uint16_t index = 0;
				for (size_t i = 0; i < Gcodes::getGcodeCount(); i++)
				{
					const gcode* g = Gcodes::getGcode(i);
					if (std::string(g->gcode).rfind(upper_cmd, 0) == 0)
					{
						lv_table_set_cell_value(view->m_commandList, index, 0, g->gcode);
						lv_table_set_cell_value(view->m_commandList, index, 1, g->helpText);
						index++;
					}
				}

				lv_table_set_row_count(view->m_commandList, index);
			}

			break;
		}
		case LV_EVENT_READY:
		{
			const char* text = lv_textarea_get_text(view->m_input);
			if (strlen(text) > 0)
			{
				// Send the command
				view->addCommand(text);
				view->m_presenter->sendGcode(text);
			}
			break;
		}

		default:
			break;
		}

		if (code == LV_EVENT_CANCEL)
		{
			lv_obj_add_flag(view->m_kb, LV_OBJ_FLAG_HIDDEN);
		}
	}

	bool ConsoleView::back()
	{
		return false;
	}

	void ConsoleView::onShow()
	{
		UI_LOCK();
		lv_obj_scroll_to_x(m_commandList, 0, LV_ANIM_OFF);
		lv_obj_add_flag(m_kb, LV_OBJ_FLAG_HIDDEN);
	}

	void ConsoleView::onHide()
	{
		// We want the console output to still update with new replies even when the console is hidden.
		activate();
	}
} // namespace UI

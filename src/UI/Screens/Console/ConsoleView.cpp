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
		, m_topCont("top_cont", getRoot())
		, m_commandList(lv_table_create, "command_list", m_topCont)
		, m_output("output", m_topCont)
		, m_inputCont("input_cont", getRoot())
		, m_input("input", m_inputCont)
		, m_clear("clear", m_input, LV_SYMBOL_CLOSE)
		, m_enter("enter", m_inputCont, LV_SYMBOL_NEW_LINE)
		, m_kb("keyboard", getRoot())
	{
		UI_LOCK();

		// Layout
		setAlign(LV_ALIGN_CENTER, 0, 0);
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_topCont.setWidth(LV_PCT(100));
		m_topCont.setFlexGrow(1);
		m_inputCont.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		// Top Container
		m_topCont.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_commandList.setFlexGrow(2);
		m_output.setFlexGrow(3);
		m_commandList.setHeight(LV_PCT(100));
		m_output.setHeight(LV_PCT(100));
		m_output.setCursorClickPos(false);

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
		m_inputCont.setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_inputCont.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_inputCont.addStyle(Themes::getLvglStyles().pad_zero);

		m_input.setFlexGrow(1);
		m_input.setOneLine(true);
		m_input.setPlaceholderText(_("console_input_placeholder"));
		m_input.setStyleTextAlign(LV_TEXT_ALIGN_LEFT, 0);
		m_clear.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_input.setHeight(LV_SIZE_CONTENT);
		m_clear.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_enter.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		m_input.addStyle(Themes::getLvglStyles().pad_zero);
		m_clear.addStyle(Themes::getLvglStyles().pad_zero);
		m_enter.addStyle(Themes::getLvglStyles().pad_zero);

		// Hide keyboard initially
		m_kb.setSize(LV_PCT(100), LV_PCT(40));
		m_kb.setMode(LV_KEYBOARD_MODE_TEXT_UPPER);
		m_kb.hide();

		// Callbacks
		m_clear.addClickedCallback(onClearEvent, this);
		m_enter.addClickedCallback(onSendEvent, this);
		m_commandList.addEventCallback(onCommandListEvent, LV_EVENT_ALL, this);
		m_input.addEventCallback(onKeyboardEvent, LV_EVENT_ALL, this);
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
			view->m_kb.setTextArea(&view->m_input);
			view->m_kb.setMode(LV_KEYBOARD_MODE_TEXT_UPPER);
			view->m_kb.show(true);
			break;
		}
		case LV_EVENT_DEFOCUSED:
		{
			view->m_kb.setTextArea(nullptr);
			view->m_kb.hide();
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

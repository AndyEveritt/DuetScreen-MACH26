#pragma once

#include "ConsolePresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvCheckbox.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvKeyboard.h"
#include "UI/Components/LVGL/LvTextarea.h"
#include "UI/Core/View.h"

namespace UI
{
	class ConsoleView : public View<ConsolePresenter>
	{
	  public:
		friend class ConsoleItem;
		friend class ConsolePresenter;

		ConsoleView(const std::string& name, LvObj& parent);

		void clear();
		void addCommand(std::string_view resp);
		void addResponse(const std::string& resp);
		void showCommandList(bool show, bool animate = LV_ANIM_ON);
		void showKeyboard(bool show);

		bool back() override;

	  private:
		static void onSendEvent(lv_event_t* e);
		static void onClearEvent(lv_event_t* e);
		static void onCommandListEvent(lv_event_t* e);
		static void onKeyboardEvent(lv_event_t* e);

		void updateBtnPos();

		void onShow() override;
		void onHide() override;

		LvContainer m_topCont{"top_cont", getRoot()};
		LvObj m_commandList{lv_table_create, "command_list", m_topCont};
		Button m_commandVisibility{"command_visibility", m_topCont, LV_SYMBOL_LIST};
		LvTextarea m_output{"output", m_topCont};
		LvContainer m_inputCont{"input_cont", getRoot()};
		LvTextarea m_input{"input", m_inputCont};
		Button m_clear{"clear", m_inputCont};
		Button m_enter{"enter", m_inputCont};
		LvKeyboard m_kb{"keyboard", getRoot()};

#if ENABLE_CONSOLE_SHELL
		LvCheckbox m_shellToggle{"shell_toggle", getRoot()};
#endif
	};
} // namespace UI
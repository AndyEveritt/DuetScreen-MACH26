#pragma once

#include "ConsolePresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvKeyboard.h"
#include "UI/Components/LVGL/LvTextArea.h"
#include "UI/Core/View.h"

namespace UI
{
	class ConsoleView : public View<ConsolePresenter>
	{
	  public:
		friend class ConsoleItem;
		friend class ConsolePresenter;

		ConsoleView(lv_obj_t* parent);

		void clear();
		void addCommand(const char* resp);
		void addResponse(const char* resp);

		bool back() override;

	  private:
		static void onSendEvent(lv_event_t* e);
		static void onClearEvent(lv_event_t* e);
		static void onCommandListEvent(lv_event_t* e);
		static void onKeyboardEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		LvContainer m_topCont;
		LvObj m_commandList;
		LvTextArea m_output;
		LvContainer m_inputCont;
		LvTextArea m_input;
		Button m_clear;
		Button m_enter;
		LvKeyboard m_kb;
	};
} // namespace UI
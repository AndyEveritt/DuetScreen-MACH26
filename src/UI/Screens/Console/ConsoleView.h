#pragma once

#include "ConsolePresenter.h"
#include "UI/Components/Button.h"
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
		void addResponse(const char* resp);

		bool back() override;

	  private:
		virtual void onShow() override;
		virtual void onHide() override;

		lv_obj_t* m_topCont;
		lv_obj_t* m_commandList;
		lv_obj_t* m_output;
		lv_obj_t* m_inputCont;
		lv_obj_t* m_input;
		Button m_clear;
		Button m_enter;
		lv_obj_t* m_kb;
	};
} // namespace UI
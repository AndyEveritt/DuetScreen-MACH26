/*
 * NumberPad.h
 *
 *  Created on: 2025-01-17
 *      Author: Andy Everitt
 */

#pragma once

#include "NumberPadPresenter.h"
#include "UI/Components/Button.h"
#include "UI/Core/View.h"

namespace UI
{
	class NumberPad : public BaseView
	{

	  public:
		NumberPad(const std::string& name, lv_obj_t* parent, layout_t layout);

	  private:
		static void btnmEventHandler(lv_event_t* e);

		lv_obj_t* m_textCont;
		lv_obj_t* m_textArea;
		Button m_clearBtn;
		lv_obj_t* m_btnMatrix;
	};
} // namespace UI

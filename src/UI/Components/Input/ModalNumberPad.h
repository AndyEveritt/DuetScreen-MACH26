/*
 * ModalNumberPad.h
 *
 *  Created on: 2025-06-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Input/NumberPad.h"

namespace UI
{
	class ModalNumberPad : public NumberPad
	{
	  public:
		ModalNumberPad(const std::string& name, lv_obj_t* parent, layout_t layout);

		void close();

	  private:
		static void modalBgEventHandler(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		LvContainer m_modalBg;
	};
} // namespace UI

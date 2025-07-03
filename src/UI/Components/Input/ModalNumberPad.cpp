/*
 * ModalNumberPad.cpp
 *
 *  Created on: 2025-06-16
 *      Author: Andy Everitt
 */

#include "ModalNumberPad.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"

namespace UI
{
	ModalNumberPad::ModalNumberPad(const std::string& name, lv_obj_t* parent, layout_t layout)
		: NumberPad(name, parent, layout)
		, m_modalBg(name + "_modal_bg", parent, layout_t(0, 0, 100, 100))
	{
		setAlign(LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_parent(getRoot(), m_modalBg);
		lv_obj_set_style_bg_opa(m_modalBg, LV_OPA_70, LV_PART_MAIN);
		// setFlag(LV_OBJ_FLAG_FLOATING, true);

		m_modalBg.addEventCallback(modalBgEventHandler, LV_EVENT_CLICKED, this);
	}

	void ModalNumberPad::close()
	{
		closeModal(this);
	}

	void ModalNumberPad::onShow()
	{
		m_modalBg.setFlag(LV_OBJ_FLAG_HIDDEN, false);
	}

	void ModalNumberPad::onHide()
	{
		m_modalBg.setFlag(LV_OBJ_FLAG_HIDDEN, true);
	}

	void ModalNumberPad::modalBgEventHandler(lv_event_t* e)
	{
		UI_LOCK();

		ModalNumberPad* modal = (ModalNumberPad*)lv_event_get_user_data(e);
		if (modal)
		{
			modal->close();
		}
	}
} // namespace UI

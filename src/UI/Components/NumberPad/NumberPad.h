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
	struct NumberPadConfig
	{
		int16_t minValue;
		int16_t maxValue;
	};

	class NumberPad : public BaseView
	{

	  public:
		NumberPad(const std::string& name, lv_obj_t* parent, layout_t layout);
		NumberPad(const std::string& name, lv_obj_t* parent, layout_t layout, const NumberPadConfig& config);

		virtual bool back();
		void clear();
		void close();
		void confirm();
		void setMinValue(int16_t value);
		void setMaxValue(int16_t value);
		void setValue(int16_t value);
		int16_t getValue() const;
		bool validateInput() const;

		void setCloseOnConfirm(bool closeOnConfirm) { m_closeOnConfirm = closeOnConfirm; }
		bool getCloseOnConfirm() const { return m_closeOnConfirm; }

		void setValueChangedCallback(lv_event_cb_t eventCb, void* userData);
		void setConfirmCallback(lv_event_cb_t eventCb, void* userData);

	  private:
		static void clearBtnEventHandler(lv_event_t* e);
		static void btnmEventHandler(lv_event_t* e);

		lv_obj_t* m_textCont;
		lv_obj_t* m_textArea;
		Button m_clearBtn;
		lv_obj_t* m_btnMatrix;

		int16_t m_minValue = INT16_MIN;
		int16_t m_maxValue = INT16_MAX;
		bool m_closeOnConfirm = true;
	};
} // namespace UI

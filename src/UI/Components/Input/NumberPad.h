/*
 * NumberPad.h
 *
 *  Created on: 2025-01-17
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/LVGL/LvButtonMatrix.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Core/View.h"

namespace UI
{
	struct NumberPadConfig
	{
		int16_t minValue;
		int16_t maxValue;
	};

	class NumberPad : public LvContainer
	{

	  public:
		using confirm_cb_t = std::function<void(float value)>;

		NumberPad(const std::string& name, LvObj& parent, layout_t layout);
		NumberPad(const std::string& name, LvObj& parent, layout_t layout, const NumberPadConfig& config);

		virtual bool back();
		void clear();
		void close();
		void confirm();
		void setHeader(std::string_view text);
		void setMinValue(float value);
		void setMaxValue(float value);
		void setRange(float minValue, float maxValue);
		void setValue(float value);
		float getValue() const;
		bool validateInput();
		void setConfirmIsAction(bool isAction);

		void setCloseOnConfirm(bool closeOnConfirm) { m_closeOnConfirm = closeOnConfirm; }
		bool getCloseOnConfirm() const { return m_closeOnConfirm; }

		void setConfirmCallback(confirm_cb_t eventCb);

	  private:
		static void clearBtnEventHandler(lv_event_t* e);
		static void onReadyEventHandler(lv_event_t* e);
		static void btnmEventHandler(lv_event_t* e);

		LvLabel m_header{"header", getRoot()};
		LvContainer m_textCont{"textcont", getRoot()};
		TextBox m_textBox{"textarea", m_textCont};
		Button m_clearBtn{"Clear", m_textCont, LV_SYMBOL_TRASH, layout_t(75, 0, 20, 80)};
		LvButtonMatrix m_btnMatrix{"btnmatrix", getRoot()};

		confirm_cb_t m_confirmCb = nullptr;

		float m_minValue = INT16_MIN;
		float m_maxValue = INT16_MAX;
		bool m_closeOnConfirm = true;
	};
} // namespace UI

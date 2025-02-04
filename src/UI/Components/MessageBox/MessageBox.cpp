/*
 * MessageBox.cpp
 *
 *  Created on: 2025-01-29
 *      Author: Andy Everitt
 */

#include "MessageBox.h"
#include "Debug.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	static const float s_jogAmounts[6] = {-2.0f, -0.2f, -0.02f, 0.02f, 0.2f, 2.0f};

	MessageBox::MessageBox(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_msgBox(lv_msgbox_create(getCont()))
		, m_layoutColDsc{LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_title(lv_msgbox_add_title(m_msgBox, "title"))
		, m_topCont(lv_obj_create(lv_msgbox_get_content(m_msgBox)))
		, m_centralCont(lv_obj_create(lv_msgbox_get_content(m_msgBox)))
		, m_bottomCont(lv_obj_create(lv_msgbox_get_content(m_msgBox)))
		, m_text(lv_label_create(m_topCont))
		, m_image(lv_image_create(m_topCont))
		, m_inputCont(lv_obj_create(m_centralCont))
		, m_axisJogCont(lv_obj_create(m_centralCont))
		, m_choicesList(lv_obj_create(m_centralCont))
		, m_warningText(lv_label_create(m_inputCont))
		, m_minText(lv_label_create(m_inputCont))
		, m_maxText(lv_label_create(m_inputCont))
		, m_input(lv_textarea_create(m_inputCont))
		, m_cancelBtn("msgbox_cancel", m_bottomCont, _("msgbox_cancel"))
		, m_okBtn("msgbox_ok", m_bottomCont, _("msgbox_ok"))
		, m_progress(lv_bar_create(lv_msgbox_get_content(m_msgBox)))
		, m_kb(nullptr)
	{
		init();
	}

	void MessageBox::init()
	{
		Lock lock;

		// Layout
		// lv_obj_set_style_max_height(getCont(), LV_SIZE_CONTENT, 0);
		// lv_obj_set_style_max_height(m_msgBox, LV_SIZE_CONTENT, 0);
		// lv_obj_set_style_max_height(m_centralCont, LV_SIZE_CONTENT, 0);
		lv_obj_set_size(m_msgBox, LV_PCT(100), LV_SIZE_CONTENT);

		for (size_t i = 0; i < lv_obj_get_child_count(lv_msgbox_get_content(m_msgBox)); i++)
		{
			lv_obj_t* child = lv_obj_get_child(lv_msgbox_get_content(m_msgBox), i);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_height(child, LV_SIZE_CONTENT);
			// lv_obj_set_flex_align(child, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
			lv_obj_set_style_pad_all(child, 5, 0);
		}
		lv_obj_set_style_max_height(m_centralCont, LV_PCT(70), 0);
		// lv_obj_set_flex_grow(m_centralCont, 1);

		lv_obj_set_flex_flow(m_topCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_flow(m_centralCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_flow(m_bottomCont, LV_FLEX_FLOW_ROW);

		// Top Container
		for (size_t i = 0; i < lv_obj_get_child_count(m_topCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_topCont, i);
			lv_obj_set_flex_grow(child, 1);
			lv_obj_set_height(child, LV_SIZE_CONTENT);
		}

		// Central Container

		for (size_t i = 0; i < lv_obj_get_child_count(m_centralCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_centralCont, i);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_height(child, LV_SIZE_CONTENT);
		}
		// lv_obj_set_flex_grow(m_axisJogCont, 1);
		lv_obj_set_style_max_height(m_choicesList, 150, LV_PART_MAIN);

		// Input Container

		lv_obj_set_layout(m_inputCont, LV_LAYOUT_GRID);
		static const int32_t inputColDsc[] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		static const int32_t inputRowDsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		lv_obj_set_grid_dsc_array(m_inputCont, inputColDsc, inputRowDsc);
		lv_obj_set_grid_cell(m_warningText, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
		lv_obj_set_grid_cell(m_minText, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_input, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_maxText, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		for (size_t i = 0; i < lv_obj_get_child_count(m_inputCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_inputCont, i);
			lv_obj_set_style_text_align(child, LV_TEXT_ALIGN_CENTER, 0);
			lv_obj_set_height(child, LV_SIZE_CONTENT);
		}
		lv_obj_set_style_pad_all(m_input, 0, 0);
		lv_textarea_set_one_line(m_input, true);

		// Axis Jog Container
		lv_obj_set_flex_flow(m_axisJogCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(m_axisJogCont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_column(m_axisJogCont, 2, 0);

		// Choices List
		lv_obj_set_flex_flow(m_choicesList, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_choicesList, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_all(m_choicesList, 2, 0);
		lv_obj_set_style_pad_column(m_choicesList, 2, 0);

		// Bottom Container
		for (size_t i = 0; i < lv_obj_get_child_count(m_bottomCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_bottomCont, i);
			lv_obj_set_flex_grow(child, 1);
			lv_obj_set_height(child, LV_SIZE_CONTENT);
		}

		m_okBtn.setCallback(onOkEvent, LV_EVENT_CLICKED, this);
		m_cancelBtn.setCallback(onCancelEvent, LV_EVENT_CLICKED, this);

		clear();
		setMode(OM::Alert::Mode::None);
	}

	void MessageBox::ok()
	{
		Lock lock;
		if (m_okCb)
		{
			info("Calling ok callback");
			m_okCb();
		}
		close();
	}

	void MessageBox::cancel()
	{
		Lock lock;
		if (m_cancelCb)
		{
			info("Calling cancel callback");
			m_cancelCb();
		}
		close();
	}

	void MessageBox::close()
	{
		Lock lock;
		if (m_closeCb)
		{
			info("Calling close callback");
			m_closeCb();
		}
	}

	void MessageBox::setTitle(const std::string& text)
	{
		Lock lock;
		lv_label_set_text(m_title, text.c_str());
	}

	void MessageBox::setText(const std::string& text)
	{
		Lock lock;
		lv_label_set_text(m_text, text.c_str());
	}

	void MessageBox::setOkBtnText(const std::string& text)
	{
		Lock lock;
		m_okBtn.setText(text.c_str());
	}

	void MessageBox::setCancelBtnText(const std::string& text)
	{
		Lock lock;
		m_cancelBtn.setText(text.c_str());
	}

	void MessageBox::setKeyboard(lv_obj_t* kb)
	{
		Lock lock;
		info("Setting keyboard %p", kb);
		m_kb = kb;
	}

	bool MessageBox::isOpen() const
	{
		return !lv_obj_has_flag(m_msgBox, LV_OBJ_FLAG_HIDDEN);
	}

	bool MessageBox::isBlocking() const
	{
		switch (m_mode)
		{
		case OM::Alert::Mode::InfoConfirm:
		case OM::Alert::Mode::ConfirmCancel:
		case OM::Alert::Mode::Choices:
		case OM::Alert::Mode::NumberInt:
		case OM::Alert::Mode::NumberFloat:
		case OM::Alert::Mode::Text:
			// These alerts are blocking
			return true;
		default:
			break;
		}
		return false;
	}

	bool MessageBox::isResponse() const
	{
		return m_mode == OM::Alert::Mode::None;
	}

	void MessageBox::clear()
	{
		Lock lock;
		m_mode = OM::Alert::Mode::None;
		lv_label_set_text(m_title, "");
		lv_label_set_text(m_text, "");
		lv_label_set_text(m_warningText, "");
		setTitle("");
		setText("");
		setMinTextf("");
		setMaxTextf("");
		setWarningTextf("");
		setOkBtnText(_("msgbox_ok"));
		setCancelBtnText(_("msgbox_cancel"));
		warningTextVisible(false);
		minTextVisible(false);
		maxTextVisible(false);
		inputVisible(false);
		axisJogVisible(false);
		selectionVisible(false);
		okVisible(false);
		cancelVisible(false);
		progressVisible(false);
		imageVisible(false);
	}

	void MessageBox::setMode(OM::Alert::Mode mode)
	{
		Lock lock;
		info("Seting mode to %u", (uint8_t)mode);
		m_mode = mode;

		// Hide all containers
		inputVisible(false);
		axisJogVisible(false);
		selectionVisible(false);
		okVisible(false);
		cancelVisible(false);
		warningTextVisible(false);
		progressVisible(false);
		imageVisible(false);

		switch (mode)
		{
		case OM::Alert::Mode::None:
			cancelVisible(true);
			break;
		case OM::Alert::Mode::Info:
			break;
		case OM::Alert::Mode::InfoClose:
			cancelVisible(true);
			break;
		case OM::Alert::Mode::InfoConfirm:
			okVisible(true);
			break;
		case OM::Alert::Mode::ConfirmCancel:
			okVisible(true);
			cancelVisible(true);
			break;
		case OM::Alert::Mode::Choices:
			selectionVisible(true);
			break;
		case OM::Alert::Mode::NumberInt:
			inputVisible(true);
			break;
		case OM::Alert::Mode::NumberFloat:
			inputVisible(true);
			break;
		case OM::Alert::Mode::Text:
			inputVisible(true);
			break;
		}
	}

	void MessageBox::imageVisible(bool visible)
	{
		lv_obj_set_flag(m_image, LV_OBJ_FLAG_HIDDEN, !visible);
	}

	void MessageBox::okVisible(bool visible)
	{
		lv_obj_set_flag(m_okBtn.getCont(), LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::cancelVisible(bool visible)
	{
		lv_obj_set_flag(m_cancelBtn.getCont(), LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::selectionVisible(bool visible)
	{
		lv_obj_set_flag(m_choicesList, LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::inputVisible(bool visible)
	{
		lv_obj_set_flag(m_inputCont, LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::warningTextVisible(bool visible)
	{
		lv_obj_set_flag(m_warningText, LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::minTextVisible(bool visible)
	{
		lv_obj_set_flag(m_minText, LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::maxTextVisible(bool visible)
	{
		lv_obj_set_flag(m_maxText, LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::axisJogVisible(bool visible)
	{
		lv_obj_set_flag(m_axisJogCont, LV_OBJ_FLAG_HIDDEN, !visible);
		updateVisibility();
	}

	void MessageBox::progressVisible(bool visible)
	{
		lv_obj_set_flag(m_progress, LV_OBJ_FLAG_HIDDEN, !visible);
	}

	void MessageBox::updateVisibility()
	{
		bool visible = false;
		for (size_t i = 0; i < lv_obj_get_child_count(m_centralCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_centralCont, i);
			if (!lv_obj_has_flag(child, LV_OBJ_FLAG_HIDDEN))
			{
				visible = true;
				break;
			}
		}
		lv_obj_set_flag(m_centralCont, LV_OBJ_FLAG_HIDDEN, !visible);

		visible = false;
		for (size_t i = 0; i < lv_obj_get_child_count(m_bottomCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_bottomCont, i);
			if (!lv_obj_has_flag(child, LV_OBJ_FLAG_HIDDEN))
			{
				visible = true;
				break;
			}
		}
		lv_obj_set_flag(m_bottomCont, LV_OBJ_FLAG_HIDDEN, !visible);
	}

	void MessageBox::setWarningTextf(const char* format, ...)
	{
		Lock lock;
		va_list args;
		va_start(args, format);
		std::string txt = utils::vformat(format, args);
		va_end(args);
		lv_label_set_text(m_warningText, txt.c_str());
	}

	void MessageBox::setMinTextf(const char* format, ...)
	{
		Lock lock;
		va_list args;
		va_start(args, format);
		std::string txt = utils::vformat(format, args);
		va_end(args);
		lv_label_set_text(m_minText, txt.c_str());
	}

	void MessageBox::setMaxTextf(const char* format, ...)
	{
		Lock lock;
		va_list args;
		va_start(args, format);
		std::string txt = utils::vformat(format, args);
		va_end(args);
		lv_label_set_text(m_maxText, txt.c_str());
	}

	size_t MessageBox::getJogAxisCount() const
	{
		return m_axisJogList.size();
	}

	void MessageBox::setJogAxisCount(size_t count)
	{
		Lock lock;
		if (count == getJogAxisCount())
		{
			return;
		}
		if (count < getJogAxisCount())
		{
			m_axisJogList.resize(count);
			return;
		}

		m_axisJogList.reserve(count);
		for (size_t i = getJogAxisCount(); i < count; ++i)
		{
			m_axisJogList.emplace_back(std::make_unique<AxisJog>(i, m_axisJogCont, *this));
		}
	}

	const char* MessageBox::getJogAxisLetter(int index) const
	{
		return m_axisJogList[index]->getAxisLetter();
	}

	void MessageBox::setJogAxisLetter(size_t index, char letter)
	{
		if (index >= getJogAxisCount())
		{
			error("Index %u out of range", index);
			return;
		}
		m_axisJogList[index]->setAxisLetter(letter);
	}

	size_t MessageBox::getChoiceCount() const
	{
		return m_choices.size();
	}

	void MessageBox::setChoiceCount(size_t count)
	{
		Lock lock;
		if (count == getChoiceCount())
		{
			return;
		}
		if (count < getChoiceCount())
		{
			m_choices.resize(count);
			return;
		}

		m_choices.reserve(count);
		for (size_t i = getChoiceCount(); i < count; ++i)
		{
			m_choices.emplace_back(std::make_shared<Button>(
				utils::format("msgbox_choice_%u", i), m_choicesList, "", layout_t(0, 0, 20, 100)));
			m_choices[i]->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(i)));
			m_choices[i]->setCallback(onChoiceEvent, LV_EVENT_CLICKED, this);
		}
	}

	void MessageBox::setChoice(size_t index, const std::string& text)
	{
		if (index >= getChoiceCount())
		{
			error("Index %u out of range", index);
			return;
		}
		m_choices[index]->setText(text.c_str());
	}

	void MessageBox::setTimeout(uint32_t timeout)
	{
		Lock lock;
		m_timeout = timeout;
		if (timeout == 0)
		{
			if (m_timeoutTimer)
			{
				lv_timer_delete(m_timeoutTimer);
				m_timeoutTimer = nullptr;
			}
			return;
		}
		m_timeoutTimer = lv_timer_create(
			[](lv_timer_t* timer)
			{
				MessageBox* msgBox = static_cast<MessageBox*>(lv_timer_get_user_data(timer));
				msgBox->cancel();
			},
			timeout,
			this);
		lv_timer_set_repeat_count(m_timeoutTimer, 1);
	}

	void MessageBox::onOkEvent(lv_event_t* e)
	{
		Lock lock;
		MessageBox* msgBox = static_cast<MessageBox*>(lv_event_get_user_data(e));
		msgBox->ok();
	}

	void MessageBox::onCancelEvent(lv_event_t* e)
	{
		Lock lock;
		MessageBox* msgBox = static_cast<MessageBox*>(lv_event_get_user_data(e));
		msgBox->cancel();
	}

	void MessageBox::onChoiceEvent(lv_event_t* e)
	{
		Lock lock;
		MessageBox* msgBox = static_cast<MessageBox*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
		uintptr_t index = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
		if (msgBox->m_choiceCb)
		{
			msgBox->m_choiceCb(index);
		}
		msgBox->close();
	}

	MessageBox::AxisJog::AxisJog(const size_t index, lv_obj_t* parent, MessageBox& msgBox)
		: BaseView(utils::format("msgbox_axis_jog_%u", index), parent)
		, m_index(index)
		, m_msgBox(msgBox)
		, m_label(lv_label_create(getCont()))
		, m_relMove{Button(utils::format("msgbox_axis_%u_rel_move_1", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("msgbox_axis_%u_rel_move_2", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("msgbox_axis_%u_rel_move_3", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("msgbox_axis_%u_rel_move_4", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("msgbox_axis_%u_rel_move_5", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("msgbox_axis_%u_rel_move_6", index), getCont(), "", layout_t(0, 0, 0, 100))}

	{
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(getCont(), 2, 0);
		lv_obj_set_style_pad_column(getCont(), 2, 0);

		for (size_t i = 0; i < ARRAY_SIZE(m_relMove); i++)
		{
			Button& btn = m_relMove[i];
			btn.setText(utils::format("%.2f", s_jogAmounts[i]).c_str());
			lv_obj_set_style_text_align(btn.getCont(), LV_TEXT_ALIGN_CENTER, 0);
			lv_obj_set_flex_grow(btn.getCont(), 1);
			lv_obj_set_height(btn.getCont(), LV_SIZE_CONTENT);
			btn.setUserData(reinterpret_cast<void*>(const_cast<float*>(&s_jogAmounts[i])));
		}

		lv_obj_set_style_text_align(m_label, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_flex_grow(m_label, 3);
		lv_obj_move_to_index(m_label, 3);
	}

	void MessageBox::AxisJog::setAxisLetter(char letter)
	{
		Lock lock;
		m_axisLetter[0] = letter;
		m_axisLetter[1] = '\0';
		lv_label_set_text(m_label, utils::format(_("msgbox_axis_position"), m_axisLetter, m_position).c_str());
	}

	void MessageBox::AxisJog::setPosition(float position)
	{
		Lock lock;
		m_position = position;
		lv_label_set_text(m_label, utils::format(_("msgbox_axis_position"), m_axisLetter, m_position).c_str());
	}

} // namespace UI

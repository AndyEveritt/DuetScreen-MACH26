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
	{
		init();
		setJogAxisCount(1);
		setChoiceCount(10);
		setChoice(2, "Choice 1");
	}

	void MessageBox::init()
	{
		Lock lock;

		// Layout
		lv_obj_set_style_max_height(getCont(), LV_SIZE_CONTENT, 0);
		lv_obj_set_style_max_height(m_msgBox, LV_SIZE_CONTENT, 0);
		lv_obj_set_style_max_height(m_centralCont, LV_SIZE_CONTENT, 0);
		lv_obj_set_size(m_msgBox, LV_PCT(100), LV_PCT(100));
		lv_msgbox_add_close_button(m_msgBox);

		for (size_t i = 0; i < lv_obj_get_child_count(lv_msgbox_get_content(m_msgBox)); i++)
		{
			lv_obj_t* child = lv_obj_get_child(lv_msgbox_get_content(m_msgBox), i);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_height(child, LV_SIZE_CONTENT);
			// lv_obj_set_flex_align(child, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
			lv_obj_set_style_pad_all(child, 5, 0);
		}
		lv_obj_set_flex_grow(m_centralCont, 1);

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

	void MessageBox::onChoiceEvent(lv_event_t* e)
	{
		Lock lock;
		MessageBox* msgBox = static_cast<MessageBox*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
		uintptr_t index = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
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

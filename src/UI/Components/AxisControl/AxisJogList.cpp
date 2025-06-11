/*
 * AxisJogList.cpp
 *
 *  Created on: 2025-06-11
 *      Author: Andy Everitt
 */

#include "AxisJogList.h"
#include "Debug.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	AxisItem::AxisItem(const size_t index, lv_obj_t* parent)
		: ListItem("move_axis_item", index, parent)
		, m_home(utils::format("move_axis_%u_home", index), getCont(), "", layout_t(0, 0, 0, 100))
		, m_relMove(utils::format("move_axis_%u_rel_move", index), getCont())
		, m_toolPosition("tool_position", getCont())
		, m_machinePosition("machine_position", getCont())
	{
		// Layout
		UI_LOCK();
		constexpr lv_coord_t pad = 2;
		lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(getCont(), pad, 0);
		lv_obj_set_style_pad_column(getCont(), pad, 0);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_home.getCont(), 4);

		m_relMove.setListFlow(LV_FLEX_FLOW_ROW);
		m_relMove.setStylePad(0);
		m_relMove.setListPad(0);
		m_relMove.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_relMove.setListSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		lv_obj_set_flex_grow(m_toolPosition, 5);
		lv_obj_set_flex_grow(m_machinePosition, 5);
		lv_obj_set_style_text_align(m_toolPosition, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_machinePosition, LV_TEXT_ALIGN_CENTER, 0);

		lv_label_set_text(m_toolPosition, "");
		lv_label_set_text(m_machinePosition, "");

		m_home.setCallback(onHomeEvent, LV_EVENT_CLICKED, this);
		m_home.addStyle(Themes::getLvglStyles().actionBtn, 0);

		// Styles
		m_home.setCheckable(true);
		m_home.addStyle(Themes::getComponentStyles().unhomed, LV_STATE_CHECKED);
	}

	AxisItem::~AxisItem() {}

	void AxisItem::setAxisLetter(const char* letter)
	{
		UI_LOCK();
		m_axisLetter = letter;
		m_home.setText(utils::format(_("move_axis_home"), letter).c_str());
	}

	void AxisItem::setJogAmounts(const float* distances, const size_t count)
	{
		UI_LOCK();
		m_relMove.clear();

		if (count == 0)
		{
			return;
		}

		if (distances == nullptr)
		{
			LOG_ERROR("distances is null");
			return;
		}

		m_relMove.setItemCount(count,
							   [&distances, this](size_t i, lv_obj_t* parent)
							   {
								   auto btn = std::make_shared<Button>(
									   utils::format("axis_%u_move_rel_button_%u", getIndex(), i),
									   parent,
									   utils::format("%.1f", distances[i]).c_str());
								   btn->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(i)));
								   btn->setCallback(onRelMoveEvent, LV_EVENT_CLICKED, this);
								   btn->setFlexGrow(1);
								   btn->setHeight(LV_SIZE_CONTENT);
								   btn->addStyle(Themes::getLvglStyles().actionBtn, 0);
								   return btn;
							   });
	}

	void AxisItem::setJogAmounts(const std::vector<float>& distances)
	{
		UI_LOCK();
		setJogAmounts(distances.data(), distances.size());
	}

	void AxisItem::setHomed(const bool homed)
	{
		UI_LOCK();
		m_home.setChecked(!homed);
	}

	void AxisItem::setToolPosition(const float& position)
	{
		UI_LOCK();
		lv_label_set_text(m_toolPosition, utils::format("%.2f", position).c_str());
	}

	void AxisItem::setMachinePosition(const float& position)
	{
		UI_LOCK();
		lv_label_set_text(m_machinePosition, utils::format("(%.2f)", position).c_str());
	}

	void AxisItem::disableHome(const bool disabled)
	{
		UI_LOCK();
		m_home.setInvalid(disabled);
		m_home.setText(disabled ? m_axisLetter.c_str()
								: utils::format(_("move_axis_home"), m_axisLetter.c_str()).c_str());
	}

	void AxisItem::setJogCallback(jog_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_jogCallback = cb;
		m_jogUserData = user_data;
	}

	void AxisItem::setHomeCallback(home_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_homeCallback = cb;
		m_homeUserData = user_data;
	}

	void AxisItem::onHomeEvent(lv_event_t* e)
	{
		UI_LOCK();
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
		if (item == nullptr)
		{
			LOG_ERROR("item is null");
			return;
		}
		if (item->m_homeCallback)
		{
			item->m_homeCallback(item->getIndex(), item->m_homeUserData);
		}
	}

	void AxisItem::onRelMoveEvent(lv_event_t* e)
	{
		UI_LOCK();
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		uintptr_t index = (uintptr_t)lv_obj_get_user_data(btn);

		if (item->m_jogCallback)
		{
			item->m_jogCallback(item->getIndex(), index, item->m_jogUserData);
		}
	}

	AxisJogList::AxisJogList(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_obj_create, name, parent)
		, m_axisItems("axis_jog_list_item", getCont())
		, m_listHeaderPadding("axis_jog_list_header_padding", m_axisItems.getHeader())
		, m_toolPositionLabel("tool_position_label", m_axisItems.getHeader())
		, m_machinePositionLabel("machine_position_label", m_axisItems.getHeader())
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

		addStyle(Themes::getLvglStyles().no_border, 0);

		m_axisItems.setListGrow(1);
		m_axisItems.setSize(LV_PCT(100), LV_PCT(100));
		m_axisItems.showHeader(true);

		Container& header = m_axisItems.getHeader();
		m_listHeaderPadding.setHeight(0);
		m_listHeaderPadding.addStyle(Themes::getLvglStyles().no_border, 0);

		m_listHeaderPadding.setFlexGrow(20);
		m_toolPositionLabel.setFlexGrow(5);
		m_machinePositionLabel.setFlexGrow(5);

		m_toolPositionLabel.setText(_("move_tool_position"));
		m_machinePositionLabel.setText(_("move_machine_position"));

		m_toolPositionLabel.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);
		m_machinePositionLabel.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);
	}
} // namespace UI

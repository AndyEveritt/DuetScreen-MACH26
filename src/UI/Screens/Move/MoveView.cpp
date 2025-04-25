#include "MoveView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	static constexpr float s_relMoveValues[] = {-50, -10, -1.0f, -0.1f, 0.1f, 1.0f, 10.0f, 50.0f}; // mm
	static constexpr uint32_t s_feedRates[] = {300, 100, 50, 20, 10, 5};						   // mm/s
	static uint32_t s_currentFeedRateIndex = 2;

	AxisItem::AxisItem(const size_t index, lv_obj_t* parent, layout_t layout)
		: BaseView(utils::format("move_axis_item_%u", index), parent, layout)
		, m_index(index)
		, m_home(utils::format("move_axis_%u_home", index), getCont(), "", layout_t(0, 0, 0, 100))
		, m_relMove{Button(utils::format("move_axis_%u_rel_move_1", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_2", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_3", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_4", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_5", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_6", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_7", index), getCont(), "", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_8", index), getCont(), "", layout_t(0, 0, 0, 100))}
		, m_toolPosition(lv_label_create(getCont()))
		, m_machinePosition(lv_label_create(getCont()))
	{
		// Layout
		UI_LOCK();
		constexpr lv_coord_t pad = 2;
		lv_obj_set_style_pad_all(getCont(), pad, 0);
		lv_obj_set_style_pad_column(getCont(), pad, 0);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_home.getCont(), 4);
		for (Button& relMove : m_relMove)
		{
			relMove.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(&relMove - m_relMove)));
			relMove.setCallback(onRelMoveEvent, LV_EVENT_CLICKED, this);
			relMove.setText(utils::format("%.1f", s_relMoveValues[&relMove - m_relMove]).c_str());
			lv_obj_set_flex_grow(relMove.getCont(), 2);
		}
		lv_obj_set_flex_grow(m_toolPosition, 5);
		lv_obj_set_flex_grow(m_machinePosition, 5);
		lv_obj_set_style_text_align(m_toolPosition, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_machinePosition, LV_TEXT_ALIGN_CENTER, 0);

		lv_label_set_text(m_toolPosition, "");
		lv_label_set_text(m_machinePosition, "");

		m_home.setCallback(onHomeEvent, LV_EVENT_CLICKED, this);

		// Styles
		m_home.setCheckable(true);
		lv_obj_set_style_bg_color(m_home.getChild(0), lv_color_hex(0xfb9514), LV_STATE_CHECKED);
	}

	AxisItem::~AxisItem() {}

	void AxisItem::setAxisLetter(const char* letter)
	{
		UI_LOCK();
		m_home.setText(utils::format(_("move_axis_home"), letter).c_str());
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

	void AxisItem::onHomeEvent(lv_event_t* e)
	{
		UI_LOCK();
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
		item->getList()->m_presenter->homeAxis(item->getIndex());
	}

	void AxisItem::onRelMoveEvent(lv_event_t* e)
	{
		UI_LOCK();
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		uintptr_t index = (uintptr_t)lv_obj_get_user_data(btn);

		float distance = s_relMoveValues[index];
		item->getList()->m_presenter->moveAxisRelative(item->getIndex(), distance, s_feedRates[s_currentFeedRateIndex]);
	}

	MoveView::MoveView(lv_obj_t* parent)
		: View("move_view", parent, layout_t(0, 0, 100, 100))
		, m_layoutColDsc{LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_FR(1), 30, LV_GRID_FR(3), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_topBarCont(lv_obj_create(getCont()))
		, m_listHeader(lv_obj_create(getCont()))
		, m_listCont(lv_obj_create(getCont()))
		, m_bottomBarCont(lv_obj_create(getCont()))
		, m_homeAll("move_home_all", m_topBarCont, _("home_all"), layout_t(0, 0, 0, 100))
		, m_trueBedLevel("move_true_bed_level", m_topBarCont, _("true_bed_level"), layout_t(0, 0, 0, 100))
		, m_meshBedLevel("move_mesh_bed_level", m_topBarCont, _("mesh_bed_level"), layout_t(0, 0, 0, 100))
		, m_heightmap("move_heightmap", m_topBarCont, _("heightmap"), layout_t(0, 0, 0, 100))
		, m_disableMotors("move_disable_motors", m_topBarCont, _("disable_motors"), layout_t(0, 0, 0, 100))
		, m_listHeaderPadding(lv_obj_create(m_listHeader))
		, m_toolPositionLabel(lv_label_create(m_listHeader))
		, m_machinePositionLabel(lv_label_create(m_listHeader))
		, m_feedRateLabel(lv_label_create(m_bottomBarCont))
		, m_feedRates{Button("move_feed_rate_1", m_bottomBarCont, "", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_2", m_bottomBarCont, "", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_3", m_bottomBarCont, "", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_4", m_bottomBarCont, "", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_5", m_bottomBarCont, "", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_6", m_bottomBarCont, "", layout_t(0, 0, 0, 100))}
	{
		UI_LOCK();

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_topBarCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_listHeader, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_listCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		lv_obj_set_grid_cell(m_bottomBarCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 3, 1);

		// Top Bar
		constexpr lv_coord_t pad = 10;
		lv_obj_set_style_pad_all(m_topBarCont, pad, 0);
		lv_obj_set_style_pad_column(m_topBarCont, pad, 0);
		lv_obj_set_flex_flow(m_topBarCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_topBarCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_homeAll.getCont(), 1);
		lv_obj_set_flex_grow(m_trueBedLevel.getCont(), 1);
		lv_obj_set_flex_grow(m_meshBedLevel.getCont(), 1);
		lv_obj_set_flex_grow(m_heightmap.getCont(), 1);
		lv_obj_set_flex_grow(m_disableMotors.getCont(), 1);

		// List Header
		lv_obj_set_flex_flow(m_listHeader, LV_FLEX_FLOW_ROW);
		lv_obj_set_style_pad_ver(m_listHeader, 0, 0);
		lv_obj_set_style_pad_column(m_listHeader, pad, 0);
		lv_obj_set_flex_grow(m_listHeaderPadding, 10);
		lv_obj_set_height(m_listHeaderPadding, LV_PCT(0));
		lv_obj_set_flex_grow(m_toolPositionLabel, 5);
		lv_obj_set_flex_grow(m_machinePositionLabel, 5);
		lv_obj_set_height(m_toolPositionLabel, LV_PCT(100));
		lv_obj_set_height(m_machinePositionLabel, LV_PCT(100));
		lv_obj_set_style_text_align(m_toolPositionLabel, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_machinePositionLabel, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(m_toolPositionLabel, _("move_tool_position"));
		lv_label_set_text(m_machinePositionLabel, _("move_machine_position"));

		m_homeAll.setCallback(onHomeAllEvent, LV_EVENT_CLICKED, this);
		m_trueBedLevel.setCallback(onTrueBedLevelEvent, LV_EVENT_CLICKED, this);
		m_meshBedLevel.setCallback(onMeshBedLevelEvent, LV_EVENT_CLICKED, this);
		m_heightmap.setCallback(onHeightmapEvent, LV_EVENT_CLICKED, this);
		m_disableMotors.setCallback(onDisableMotorsEvent, LV_EVENT_CLICKED, this);

		// List
		lv_obj_set_flex_flow(m_listCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_style_pad_row(m_listCont, 2, 0);

		// Bottom Bar
		lv_obj_set_style_pad_all(m_bottomBarCont, pad, 0);
		lv_obj_set_style_pad_column(m_bottomBarCont, pad, 0);
		lv_obj_set_flex_flow(m_bottomBarCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_bottomBarCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_feedRateLabel, 3);
		for (Button& feedRate : m_feedRates)
		{
			feedRate.setText(utils::format("%u", s_feedRates[&feedRate - m_feedRates]).c_str());
			feedRate.setCheckable(true);
			feedRate.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(&feedRate - m_feedRates)));
			feedRate.setCallback(onFeedRateEvent, LV_EVENT_CLICKED, this);
			lv_obj_set_flex_grow(feedRate.getCont(), 1);
		}
		m_feedRates[s_currentFeedRateIndex].setChecked(true);
		lv_label_set_text(m_feedRateLabel, _("move_feedrate"));
		lv_obj_set_style_text_align(m_feedRateLabel, LV_TEXT_ALIGN_RIGHT, 0);
	}

	void MoveView::onHomeAllEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->homeAll();
	}

	void MoveView::onTrueBedLevelEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->trueBedLevel();
	}

	void MoveView::onMeshBedLevelEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->meshBedLevel();
	}

	void MoveView::onHeightmapEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->heightmap();
	}

	void MoveView::onDisableMotorsEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		view->m_presenter->disableMotors();
	}

	void MoveView::onFeedRateEvent(lv_event_t* e)
	{
		UI_LOCK();
		MoveView* view = static_cast<MoveView*>(lv_event_get_user_data(e));
		lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
		view->m_feedRates[s_currentFeedRateIndex].setChecked(false);
		s_currentFeedRateIndex = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn));
		view->m_feedRates[s_currentFeedRateIndex].setChecked(true);
	}

	void MoveView::onShow() {}
	void MoveView::onHide() {}

	void MoveView::setAxisCount(const size_t count)
	{
		UI_LOCK();
		if (count == getAxisCount())
		{
			return;
		}
		if (count < getAxisCount())
		{
			m_axisItems.resize(count);
			return;
		}

		m_axisItems.reserve(count);
		for (size_t i = getAxisCount(); i < count; ++i)
		{
			m_axisItems.emplace_back(std::make_unique<AxisItem>(i, m_listCont, layout_t(0, 0, 100, 20)));
		}
	}

	std::shared_ptr<AxisItem> MoveView::getAxisItem(size_t index) const
	{
		UI_LOCK();
		if (index < m_axisItems.size())
		{
			return m_axisItems[index];
		}
		return nullptr;
	}
} // namespace UI

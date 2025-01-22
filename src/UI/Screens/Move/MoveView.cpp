#include "MoveView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	AxisItem::AxisItem(const size_t index, lv_obj_t* parent, layout_t layout)
		: BaseView(utils::format("move_axis_item_%u", index), parent, layout)
		, m_home(utils::format("move_axis_%u_home", index), getCont(), _("move_axis_home"), layout_t(0, 0, 0, 100))
		, m_relMove{Button(utils::format("move_axis_%u_rel_move_1", index), getCont(), "-50", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_2", index), getCont(), "-10", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_3", index), getCont(), "-1", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_4", index), getCont(), "-0.1", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_5", index), getCont(), "0.1", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_6", index), getCont(), "1", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_7", index), getCont(), "10", layout_t(0, 0, 0, 100)),
					Button(utils::format("move_axis_%u_rel_move_8", index), getCont(), "50", layout_t(0, 0, 0, 100))}
		, m_toolPosition(lv_label_create(getCont()))
		, m_machinePosition(lv_label_create(getCont()))
	{
		// Layout
		constexpr lv_coord_t pad = 2;
		lv_obj_set_style_pad_all(getCont(), pad, 0);
		lv_obj_set_style_pad_column(getCont(), pad, 0);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_home.getCont(), 2);
		for (Button& relMove : m_relMove)
		{
			lv_obj_set_flex_grow(relMove.getCont(), 1);
		}
		lv_obj_set_flex_grow(m_toolPosition, 3);
		lv_obj_set_flex_grow(m_machinePosition, 3);
		lv_obj_set_style_text_align(m_toolPosition, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_machinePosition, LV_TEXT_ALIGN_CENTER, 0);

		lv_label_set_text(m_toolPosition, "");
		lv_label_set_text(m_machinePosition, "");
	}

	void AxisItem::setToolPosition(const float& position)
	{
		Lock lock;
		lv_label_set_text_fmt(m_toolPosition, "%.2f", position);
	}

	void AxisItem::setMachinePosition(const float& position)
	{
		Lock lock;
		lv_label_set_text_fmt(m_machinePosition, "(%.2f)", position);
	}

	void AxisItem::onHomeEvent(lv_event_t* e)
	{
		Lock lock;
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
		item->getList();
	}

	void AxisItem::onRelMoveEvent(lv_event_t* e)
	{
		Lock lock;
		AxisItem* item = static_cast<AxisItem*>(lv_event_get_user_data(e));
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
		, m_feedRates{Button("move_feed_rate_1", m_bottomBarCont, "300", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_2", m_bottomBarCont, "100", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_3", m_bottomBarCont, "50", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_4", m_bottomBarCont, "20", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_5", m_bottomBarCont, "10", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_6", m_bottomBarCont, "5", layout_t(0, 0, 0, 100))}
	{
		Lock lock;

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
		lv_obj_set_flex_grow(m_toolPositionLabel, 3);
		lv_obj_set_flex_grow(m_machinePositionLabel, 3);
		lv_obj_set_height(m_toolPositionLabel, LV_PCT(100));
		lv_obj_set_height(m_machinePositionLabel, LV_PCT(100));
		lv_obj_set_style_text_align(m_toolPositionLabel, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_machinePositionLabel, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(m_toolPositionLabel, _("move_tool_position"));
		lv_label_set_text(m_machinePositionLabel, _("move_machine_position"));

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
			lv_obj_set_flex_grow(feedRate.getCont(), 1);
		}
		lv_label_set_text(m_feedRateLabel, _("move_feedrate"));
		lv_obj_set_style_text_align(m_feedRateLabel, LV_TEXT_ALIGN_RIGHT, 0);

		setAxisCount(5);
	}

	void MoveView::onShow() {}
	void MoveView::onHide() {}

	void MoveView::setAxisCount(const size_t count)
	{
		Lock lock;
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
} // namespace UI

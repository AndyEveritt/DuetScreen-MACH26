#include "MoveView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	MoveView::MoveView(lv_obj_t* parent)
		: View("move_view", parent, layout_t(0, 0, 100, 100))
		, m_topBarCont(lv_obj_create(getCont()))
		, m_listCont(lv_obj_create(getCont()))
		, m_bottomBarCont(lv_obj_create(getCont()))
		, m_homeAll("move_home_all", m_topBarCont, _("home_all"), layout_t(0, 0, 0, 100))
		, m_trueBedLevel("move_true_bed_level", m_topBarCont, _("true_bed_level"), layout_t(0, 0, 0, 100))
		, m_meshBedLevel("move_mesh_bed_level", m_topBarCont, _("mesh_bed_level"), layout_t(0, 0, 0, 100))
		, m_heightmap("move_heightmap", m_topBarCont, _("heightmap"), layout_t(0, 0, 0, 100))
		, m_disableMotors("move_disable_motors", m_topBarCont, _("disable_motors"), layout_t(0, 0, 0, 100))
		, m_feedRateLabel(lv_label_create(m_bottomBarCont))
		, m_feedRates{Button("move_feed_rate_1", m_bottomBarCont, "300", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_2", m_bottomBarCont, "100", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_3", m_bottomBarCont, "50", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_4", m_bottomBarCont, "20", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_5", m_bottomBarCont, "10", layout_t(0, 0, 0, 100)),
					  Button("move_feed_rate_6", m_bottomBarCont, "5", layout_t(0, 0, 0, 100))}
	{
		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_topBarCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_listCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_bottomBarCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);

		// Top Bar
		lv_obj_set_flex_flow(m_topBarCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_topBarCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_homeAll.getCont(), 1);
		lv_obj_set_flex_grow(m_trueBedLevel.getCont(), 1);
		lv_obj_set_flex_grow(m_meshBedLevel.getCont(), 1);
		lv_obj_set_flex_grow(m_heightmap.getCont(), 1);
		lv_obj_set_flex_grow(m_disableMotors.getCont(), 1);

		// List
		lv_obj_set_flex_flow(m_listCont, LV_FLEX_FLOW_COLUMN);

		// Bottom Bar
		lv_obj_set_flex_flow(m_bottomBarCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_bottomBarCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_flex_grow(m_feedRateLabel, 3);
		for (Button& feedRate : m_feedRates)
		{
			lv_obj_set_flex_grow(feedRate.getCont(), 1);
		}
		lv_label_set_text(m_feedRateLabel, _("move_feedrate"));
		lv_obj_set_style_text_align(m_feedRateLabel, LV_TEXT_ALIGN_RIGHT, 0);
	}

	void MoveView::onShow() {}
	void MoveView::onHide() {}
} // namespace UI

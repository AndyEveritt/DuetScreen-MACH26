#include "HeightmapView.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"

#include "ObjectModel/Heightmap.h"
#include <cmath>

namespace UI
{
	HeightmapView::HeightmapView(lv_obj_t* parent)
		: View("HeightmapView", parent, layout_t(0, 0, 100, 100))
		, m_layoutColDsc{LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST}
		, m_graphCont(lv_obj_create(getCont()))
		, m_listCont(lv_obj_create(getCont()))
		, m_infoCont(lv_obj_create(getCont()))
		, m_heightmap("heightmap", m_graphCont, layout_t(0, 0, 100, 100))
	{
		UI_LOCK();
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_graphCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_listCont, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_infoCont, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 1);

		lv_obj_t* cont = lv_obj_create(m_infoCont);
		lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

		lv_obj_t* m_scale = lv_scale_create(cont);
		lv_obj_set_height(m_scale, 30);
		lv_scale_set_mode(m_scale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
		lv_scale_set_label_show(m_scale, true);
		lv_obj_set_style_border_width(m_scale, 2, LV_PART_MAIN);

		lv_obj_t* m_scale2 = lv_scale_create(cont);
		lv_obj_set_height(m_scale2, LV_SIZE_CONTENT);
		lv_scale_set_mode(m_scale2, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
		lv_scale_set_label_show(m_scale2, true);
		lv_obj_set_style_border_width(m_scale2, 2, LV_PART_MAIN);

		// m_heightmap.setXRange({-200, 200});
		m_heightmap.setTitle("Heightmap");
	}

	void HeightmapView::onShow() {}
} // namespace UI

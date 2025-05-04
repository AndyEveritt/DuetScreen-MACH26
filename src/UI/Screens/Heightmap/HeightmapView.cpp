#include "HeightmapView.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"

#include "ObjectModel/Heightmap.h"
#include <cmath>

namespace UI
{
	class HeightmapItem : public ListItem
	{
	  public:
		HeightmapItem(size_t index, lv_obj_t* parent, HeightmapView& view)
			: ListItem("heightmap_item", index, parent)
			, m_view(view)
			, m_label(lv_label_create(getCont()))
		{
			UI_LOCK();
			lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);

			lv_obj_set_size(m_label, LV_PCT(100), LV_SIZE_CONTENT);
		}

		void setLabel(const std::string& label)
		{
			UI_LOCK();
			lv_label_set_text(m_label, label.c_str());
		}

	  private:
		HeightmapView& m_view;

		lv_obj_t* m_label;
	};

	HeightmapView::HeightmapView(lv_obj_t* parent)
		: View("HeightmapView", parent, layout_t(0, 0, 100, 100))
		, m_layoutColDsc{LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST}
		, m_graphCont(lv_obj_create(getCont()))
		, m_infoCont(lv_obj_create(getCont()))
		, m_heightmap("heightmap", m_graphCont, layout_t(0, 0, 100, 100))
		, m_heightmapList("heightmap_list", getCont())
	{
		UI_LOCK();
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);

		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_graphCont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_heightmapList, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_infoCont, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 1);

		// m_heightmap.setXRange({-200, 200});
		m_heightmap.setTitle("Heightmap");

		// List
		m_heightmapList.setTitle(_("heightmap_list_header"));
		m_heightmapList.setItemCount(5, *this);
		m_heightmapList.setListGrow(1);

		setHeightmapName(0, "Heightmap 0");
		setHeightmapName(1, "Heightmap 1");
		setHeightmapName(2, "Heightmap 2");
		setHeightmapName(3, "Heightmap 3");
		setHeightmapName(4, "Heightmap 4");
	}

	const size_t HeightmapView::getHeightmapCount() const
	{
		UI_LOCK();
		return m_heightmapList.getItemCount();
	}

	void HeightmapView::setHeightmapCount(const size_t count)
	{
		UI_LOCK();
		m_heightmapList.setItemCount(count, *this);
	}

	void HeightmapView::setHeightmapName(const size_t index, const std::string& name)
	{
		UI_LOCK();

		auto item = m_heightmapList.getItem(index);
		if (item == nullptr)
		{
			LOG_WARN("Can't set heightmap {:d} name to {:s}, index invalid", index, name);
			return;
		}

		item->setLabel(name);
	}

	void HeightmapView::onShow() {}
} // namespace UI

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
			, m_load("heightmap_load", getCont(), "", layout_t(0, 0, LV_SIZE_CONTENT, LV_SIZE_CONTENT))
		{
			UI_LOCK();
			setLayoutStyle(LV_LAYOUT_FLEX, LV_FLEX_FLOW_ROW);
			lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
			lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
			lv_obj_set_height(m_label, LV_SIZE_CONTENT);
			lv_obj_set_flex_grow(m_label, 1);

			lv_obj_set_style_bg_color(getCont(), lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_CHECKED);
			lv_obj_add_flag(getCont(), LV_OBJ_FLAG_CLICKABLE);
			lv_obj_add_event_cb(
				getCont(),
				[](lv_event_t* event)
				{
					UI_LOCK();
					auto item = static_cast<HeightmapItem*>(lv_event_get_user_data(event));
					if (item == nullptr)
					{
						LOG_WARN("Heightmap item is null");
						return;
					}
					item->m_view.getPresenter()->setActiveHeightmap(item->getIndex());
				},
				LV_EVENT_CLICKED,
				this);

			m_load.setCallback(
				[](lv_event_t* event)
				{
					UI_LOCK();
					auto item = static_cast<HeightmapItem*>(lv_event_get_user_data(event));
					if (item == nullptr)
					{
						LOG_WARN("Heightmap item is null");
						return;
					}
					item->m_view.getPresenter()->toggleHeightmap(item->getIndex());
					item->m_view.getPresenter()->setActiveHeightmap(item->getIndex());
				},
				LV_EVENT_CLICKED,
				this);
			// m_load.setCheckable(true);
		}

		void setLabel(const std::string& label)
		{
			UI_LOCK();
			lv_label_set_text(m_label, label.c_str());
		}

		void setSelected(bool selected)
		{
			UI_LOCK();
			lv_obj_set_state(getCont(), LV_STATE_CHECKED, selected);
			m_load.setText(selected ? _("heightmap_unload") : _("heightmap_load"));
			m_load.setChecked(selected);
		}

	  private:
		HeightmapView& m_view;

		lv_obj_t* m_label;
		Button m_load;
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
		m_heightmapList.setListGrow(1);
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

	void HeightmapView::setSelectedHeightmap(const int32_t index)
	{
		UI_LOCK();
		for (auto item : m_heightmapList)
		{
			if (item == nullptr)
			{
				LOG_WARN("Heightmap item is null");
				continue;
			}
			item->setSelected(item->getIndex() == index);
		}
	}

	void HeightmapView::onShow() {}
} // namespace UI

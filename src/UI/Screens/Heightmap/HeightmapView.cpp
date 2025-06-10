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
			setFlexFlow(LV_FLEX_FLOW_ROW);
			lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
			lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
			lv_obj_set_height(m_label, LV_SIZE_CONTENT);
			lv_obj_set_flex_grow(m_label, 1);

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

			m_load.setSize(70, LV_SIZE_CONTENT);
			addStyle(Themes::getLvglStyles().bg_color_primary, LV_STATE_CHECKED);
			m_load.addBtnStyle(Themes::getLvglStyles().actionBtn, 0);
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

	HeightmapRenderMode::HeightmapRenderMode(lv_obj_t* parent, HeightmapPresenter& presenter)
		: LvObj(lv_obj_create, "heightmap_render_mode", parent)
		, m_presenter(presenter)
		, m_title(lv_label_create(getCont()))
		, m_btns(lv_obj_create(getCont()))
		, m_fixed("heightmap_fixed", m_btns, _("heightmap_fixed"), layout_t(0, 0, LV_SIZE_CONTENT, LV_SIZE_CONTENT))
		, m_auto("heightmap_auto", m_btns, _("heightmap_auto"), layout_t(0, 0, LV_SIZE_CONTENT, LV_SIZE_CONTENT))
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		lv_label_set_text(m_title, _("heightmap_render_mode"));

		lv_obj_set_flex_flow(m_btns, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_btns, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

		lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_size(m_btns, LV_PCT(100), LV_SIZE_CONTENT);
		m_fixed.setHeight(LV_SIZE_CONTENT);
		m_auto.setHeight(LV_SIZE_CONTENT);

		m_fixed.setFlexGrow(1);
		m_auto.setFlexGrow(1);

		setRenderMode(m_presenter.getRenderMode());

		m_fixed.setCallback(
			[](lv_event_t* event)
			{
				UI_LOCK();
				auto mode = static_cast<HeightmapRenderMode*>(lv_event_get_user_data(event));
				if (mode == nullptr)
				{
					LOG_WARN("Heightmap render mode is null");
					return;
				}
				mode->m_presenter.setRenderMode(HeightmapPresenter::HeightmapRenderMode::Fixed);
				mode->setRenderMode(HeightmapPresenter::HeightmapRenderMode::Fixed);
			},
			LV_EVENT_CLICKED,
			this);

		m_auto.setCallback(
			[](lv_event_t* event)
			{
				UI_LOCK();
				auto mode = static_cast<HeightmapRenderMode*>(lv_event_get_user_data(event));
				if (mode == nullptr)
				{
					LOG_WARN("Heightmap render mode is null");
					return;
				}
				mode->m_presenter.setRenderMode(HeightmapPresenter::HeightmapRenderMode::Auto);
				mode->setRenderMode(HeightmapPresenter::HeightmapRenderMode::Auto);
			},
			LV_EVENT_CLICKED,
			this);
	}

	void HeightmapRenderMode::setRenderMode(HeightmapPresenter::HeightmapRenderMode mode)
	{
		UI_LOCK();
		switch (mode)
		{
		case HeightmapPresenter::HeightmapRenderMode::Fixed:
			m_fixed.setChecked(true);
			m_auto.setChecked(false);
			break;
		case HeightmapPresenter::HeightmapRenderMode::Auto:
			m_fixed.setChecked(false);
			m_auto.setChecked(true);
			break;
		default:
			break;
		}
	}

	HeightmapStatistics::HeightmapStatistics(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_obj_create, name, parent)
		, m_numPoints(lv_label_create(getCont()))
		, m_area(lv_label_create(getCont()))
		, m_minError(lv_label_create(getCont()))
		, m_maxError(lv_label_create(getCont()))
		, m_meanError(lv_label_create(getCont()))
		, m_stdDev(lv_label_create(getCont()))
	{
		lv_obj_set_size(getCont(), LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW_WRAP);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		for (size_t i = 0; i < lv_obj_get_child_cnt(getCont()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getCont(), i);
			lv_obj_set_size(child, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		}
	}

	void HeightmapStatistics::setStatistics(
		size_t numPoints, double area, double minError, double maxError, double meanError, double stdDev)
	{
		UI_LOCK();
		lv_label_set_text(m_numPoints, utils::format(_("heightmap_num_points"), numPoints).c_str());
		lv_label_set_text(m_area, utils::format(_("heightmap_area"), area).c_str());
		lv_label_set_text(m_minError, utils::format(_("heightmap_min_error"), minError).c_str());
		lv_label_set_text(m_maxError, utils::format(_("heightmap_max_error"), maxError).c_str());
		lv_label_set_text(m_meanError, utils::format(_("heightmap_mean_error"), meanError).c_str());
		lv_label_set_text(m_stdDev, utils::format(_("heightmap_std_dev"), stdDev).c_str());
	}

	HeightmapView::HeightmapView(lv_obj_t* parent)
		: View(lv_obj_create, "HeightmapView", parent, layout_t(0, 0, 100, 100))
		, m_layoutColDsc{LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_layoutRowDsc{LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST}
		, m_heightmap("heightmap", getCont(), layout_t(0, 0, 100, 100))
		, m_heightmapList("heightmap_list", getCont())
		, m_statistics("heightmap_statistics", getCont())
		, m_renderMode(getCont(), *getPresenter().get())
	{
		UI_LOCK();
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);

		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_heightmap, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
		lv_obj_set_grid_cell(m_heightmapList, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_renderMode, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 1, 1);
		lv_obj_set_grid_cell(m_statistics, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 2, 1);

		m_heightmap.setResolution(200, 200);

		// List
		m_heightmapList.setTitle(_("heightmap_list_header"));
		m_heightmapList.setListGrow(1);

		clear();
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

	void HeightmapView::setShownHeightmapName(const std::string& name)
	{
		UI_LOCK();
		m_heightmap.setTitle(utils::format(_("heightmap_title"), name.c_str()));
	}

	void HeightmapView::addMeasurementPoint(float x, float y)
	{
		UI_LOCK();
		size_t px, py;
		if (!m_heightmap.posToPx(x, y, px, py))
		{
			LOG_WARN("Measurement ({:g}, {:g}) not within axis bounds", x, y);
			return;
		}
		uint32_t width, height;
		m_heightmap.getResolution(width, height);
		m_heightmap.getCanvas().drawCirclePx(
			{(int32_t)px, (int32_t)(height - py)}, 1, lv_palette_main(LV_PALETTE_GREY), LV_OPA_20);
	}

	void HeightmapView::clear()
	{
		UI_LOCK();
		m_heightmap.clear();

		setShownHeightmapName("");
		drawGrid();
		setStatistics(0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}

	void HeightmapView::setStatistics(
		size_t numPoints, double area, double minError, double maxError, double meanError, double stdDev)
	{
		UI_LOCK();
		m_statistics.setStatistics(numPoints, area, minError, maxError, meanError, stdDev);
	}

	void HeightmapView::onShow() {}
} // namespace UI

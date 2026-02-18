#pragma once

#include "HeightmapPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Heatmap/Heatmap.h"
#include "UI/Components/List/List.h"
#include "UI/Core/View.h"
#include <memory>

namespace UI
{
	class HeightmapItem;

	class HeightmapRenderMode : public LvContainer
	{
	  public:
		HeightmapRenderMode(LvObj& parent, HeightmapPresenter& presenter);
		void setRenderMode(HeightmapPresenter::HeightmapRenderMode mode);

	  private:
		HeightmapPresenter& m_presenter;

		LvLabel m_title;
		LvContainer m_btns;

		Button m_fixed;
		Button m_auto;
	};

	class HeightmapStatistics : public LvContainer
	{
	  public:
		HeightmapStatistics(const std::string& name, LvObj& parent);
		void setStatistics(
			size_t numPoints, double area, double minError, double maxError, double meanError, double stdDev);

	  private:
		LvLabel m_numPoints;
		LvLabel m_area;
		LvLabel m_minError;
		LvLabel m_maxError;
		LvLabel m_meanError;
		LvLabel m_stdDev;
	};

	class HeightmapView : public View<HeightmapPresenter>
	{
	  public:
		using range_t = Heatmap::range_t;
		using range_float_t = Heatmap::range_float_t;

		HeightmapView(const std::string& name, LvObj& parent);
		virtual ~HeightmapView(); // = default but HeightmapItem is forward declared;

		/* Heightmap list methods */

		void setHeightmapCount(const size_t count);
		size_t getHeightmapCount() const;
		void setSelectedHeightmap(const size_t index);
		void setHeightmapName(const size_t index, const std::string& name);
		void setShownHeightmapName(std::string_view name);

		/* Heightmap methods */

		void setXRange(range_t range) { m_heightmap.setXRange(range); }
		void setYRange(range_t range) { m_heightmap.setYRange(range); }
		void setValueRange(float min, float max) { m_heightmap.setValueRange(min, max); }
		void getResolution(uint32_t& width, uint32_t& height) const { m_heightmap.getResolution(width, height); }
		void setResolution(uint32_t width, uint32_t height) { m_heightmap.setResolution(width, height); }
		bool pxToPos(size_t px, size_t py, float& x, float& y) const { return m_heightmap.pxToPos(px, py, x, y); }
		void setPx(size_t px, size_t py, float value) { m_heightmap.setPx(px, py, value); }
		void drawGrid() { m_heightmap.drawGrid(); }
		void renderColorBar() { m_heightmap.renderColorBar(); }
		void drawLabel(int32_t px, int32_t py, const std::string& label, lv_color_t color, lv_opa_t opa)
		{
			m_heightmap.drawLabel(px, py, label, color, opa);
		}
		void addMeasurementPoint(float x, float y);
		void clear();

		auto& getControlButtons() { return m_btnCont; }
		Heatmap& getHeightmap() { return m_heightmap; }

		/* Statistics */
		void setStatistics(
			size_t numPoints, double area, double minError, double maxError, double meanError, double stdDev);

	  private:
		static void onRefresh(lv_event_t* event);
		static void onHeightmapSelected(lv_event_t* event);
		static void onRenderModeSelected(lv_event_t* event);
		static void onTrueBedLevelEvent(lv_event_t* e);
		static void onMeshBedLevelEvent(lv_event_t* e);

		void onShow() override;

		int32_t m_layoutColDsc[3] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		int32_t m_layoutRowDsc[5] = {
			LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

		// Graph
		Heatmap m_heightmap{"heightmap", getRoot(), layout_t(0, 0, 100, 100)};

		Card m_btnCont{"control_buttons", getRoot()};
		Button m_trueBedLevel{"true_bed_level", m_btnCont};
		Button m_meshBedLevel{"mesh_bed_level", m_btnCont};

		// List
		List<HeightmapItem> m_heightmapList;

		// Statistics
		HeightmapStatistics m_statistics{"heightmap_statistics", getRoot()};

		HeightmapRenderMode m_renderMode{getRoot(), *getPresenter().get()};
	};
} // namespace UI
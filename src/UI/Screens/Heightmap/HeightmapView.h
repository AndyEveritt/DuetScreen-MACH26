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

	class HeightmapView : public View<HeightmapPresenter>
	{
	  public:
		using range_t = Heatmap::range_t;
		using range_float_t = Heatmap::range_float_t;

		HeightmapView(lv_obj_t* parent);

		/* Heightmap list methods */

		void setHeightmapCount(const size_t count);
		const size_t getHeightmapCount() const;
		void setSelectedHeightmap(const size_t index);
		void setHeightmapName(const size_t index, const std::string& name);

		/* Heightmap methods */

		void setXRange(range_t range) { m_heightmap.setXRange(range); }
		void setYRange(range_t range) { m_heightmap.setYRange(range); }
		void setValueRange(float min, float max) { m_heightmap.setValueRange(min, max); }
		void getResolution(uint32_t& width, uint32_t& height) const { m_heightmap.getResolution(width, height); }
		void setResolution(uint32_t width, uint32_t height) { m_heightmap.setResolution(width, height); }
		bool pxToPos(size_t px, size_t py, float& x, float& y) const { return m_heightmap.pxToPos(px, py, x, y); }
		void setPx(size_t px, size_t py, float value) { m_heightmap.setPx(px, py, value); }
		void renderColorBar() { m_heightmap.renderColorBar(); }
		void clear() { m_heightmap.clear(); }

		/* Statistics */
		void setStatistics(
			size_t numPoints, double area, double minError, double maxError, double meanError, double stdDev);

	  private:
		static void onRefresh(lv_event_t* event);
		static void onHeightmapSelected(lv_event_t* event);
		static void onRenderModeSelected(lv_event_t* event);

		virtual void onShow() override;

		Heatmap& getHeightmap() { return m_heightmap; }

		int32_t m_layoutColDsc[4];
		int32_t m_layoutRowDsc[3];

		// Containers
		lv_obj_t* m_graphCont;
		lv_obj_t* m_listCont;
		lv_obj_t* m_infoCont;

		// Graph
		Heatmap m_heightmap;

		// List
		List<HeightmapItem> m_heightmapList;
	};
} // namespace UI
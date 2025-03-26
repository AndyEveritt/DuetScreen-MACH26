#pragma once

#include "HeightmapPresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/Heatmap/Heatmap.h"
#include "UI/Core/View.h"

namespace UI
{
	class HeightmapView : public View<HeightmapPresenter>
	{
	  public:
		HeightmapView(lv_obj_t* parent);

	  private:
		int32_t m_layoutColDsc[4];
		int32_t m_layoutRowDsc[3];

		// Containers
		lv_obj_t* m_graphCont;
		lv_obj_t* m_listCont;
		lv_obj_t* m_infoCont;

		// Graph
		Heatmap m_heightmap;
	};
} // namespace UI
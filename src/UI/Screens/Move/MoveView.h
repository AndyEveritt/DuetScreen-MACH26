#pragma once

#include "MovePresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Core/View.h"

namespace UI
{
	class MoveView : public View<MovePresenter>
	{
	  public:
		MoveView(lv_obj_t* parent);

	  private:
		virtual void onShow() override;
		virtual void onHide() override;

		int32_t m_layoutColDsc[2] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		int32_t m_layoutRowDsc[4] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

		lv_obj_t* m_topBarCont;
		lv_obj_t* m_listCont;
		lv_obj_t* m_bottomBarCont;

		// Top Bar
		Button m_homeAll;
		Button m_trueBedLevel;
		Button m_meshBedLevel;
		Button m_heightmap;
		Button m_disableMotors;

		// Bottom Bar
		lv_obj_t* m_feedRateLabel;
		Button m_feedRates[6];
	};
} // namespace UI
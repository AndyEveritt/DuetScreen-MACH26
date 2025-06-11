#pragma once

#include "MovePresenter.h"
#include "UI/Components/AxisControl/AxisJogList.h"
#include "UI/Components/AxisControl/GenericAxisControl.h"
#include "UI/Components/AxisControl/XYControl.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/List/List.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Core/View.h"

namespace UI
{
	class MoveView : public View<MovePresenter>
	{
	  public:
		friend class AxisItem;

		MoveView(lv_obj_t* parent);

		const size_t getAxisCount() const { return m_axisList.getItemCount(); }
		void setAxisCount(const size_t count);
		std::shared_ptr<AxisItem> getAxisItem(size_t index);

	  private:
		static void onHomeAllEvent(lv_event_t* e);
		static void onTrueBedLevelEvent(lv_event_t* e);
		static void onMeshBedLevelEvent(lv_event_t* e);
		static void onHeightmapEvent(lv_event_t* e);
		static void onDisableMotorsEvent(lv_event_t* e);
		static void onFeedRateEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		int32_t m_layoutColDsc[2];
		int32_t m_layoutRowDsc[4];

		lv_obj_t* m_topBarCont;
		lv_obj_t* m_bottomBarCont;

		// Top Bar
		Button m_homeAll;
		Button m_trueBedLevel;
		Button m_meshBedLevel;
		Button m_heightmap;
		Button m_disableMotors;

		// Axis Control
		Container m_axisControlCont;
		XYControl m_xyControl;
		GenericAxisControl m_zControl;
		AxisJogList m_axisList;

		// Bottom Bar
		List<Button> m_feedRates;
	};
} // namespace UI
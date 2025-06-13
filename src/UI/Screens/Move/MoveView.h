#pragma once

#include "MovePresenter.h"
#include "UI/Components/AxisControl/AxisJogList.h"
#include "UI/Components/AxisControl/GenericAxisControl.h"
#include "UI/Components/AxisControl/XYControl.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/List/List.h"
#include "UI/Core/View.h"

namespace UI
{
	class MoveView : public View<MovePresenter>
	{
	  public:
		MoveView(lv_obj_t* parent);

		void setAxisLetters(const std::vector<char>& axis_letters);
		void setAxisPosition(char axis_letter, float position);
		void setAxisHomed(char axis_letter, bool homed);
		void setAxisDisabled(char axis_letter, bool disabled);
		void setAxisJogDisabled(char axis_letter, bool disabled);
		void setAxisHomeDisabled(char axis_letter, bool disabled);

		void clear();

	  private:
		static void onHomeAllEvent(lv_event_t* e);
		static void onTrueBedLevelEvent(lv_event_t* e);
		static void onMeshBedLevelEvent(lv_event_t* e);
		static void onHeightmapEvent(lv_event_t* e);
		static void onDisableMotorsEvent(lv_event_t* e);
		static void onDistanceEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		const size_t getAxisCount() const { return m_axisList.getItemCount(); }
		void setAxisCount(const size_t count);
		std::shared_ptr<AxisItem> getAxisItem(size_t index);

		int32_t m_layoutColDsc[2];
		int32_t m_layoutRowDsc[4];

		LvContainer m_topBarCont;
		LvContainer m_bottomBarCont;

		// Top Bar
		Button m_homeAll;
		Button m_trueBedLevel;
		Button m_meshBedLevel;
		Button m_heightmap;
		Button m_disableMotors;

		// Axis Control
		LvContainer m_axisControlCont;
		XYControl m_xyControl;
		GenericAxisControl m_zControl;
		List<GenericAxisControl> m_genericAxisControls;
		AxisJogList m_axisList;

		// Bottom Bar
		List<Button> m_distances;
	};
} // namespace UI
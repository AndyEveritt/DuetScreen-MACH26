#pragma once

#include "MovePresenter.h"
#include "UI/Components/AxisControl/AxisJogList.h"
#include "UI/Components/AxisControl/GenericAxisControl.h"
#include "UI/Components/AxisControl/XYControl.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/List/List.h"
#include "UI/Core/View.h"

namespace UI
{
	class MoveView : public View<MovePresenter>
	{
	  public:
		MoveView(const std::string& name, LvObj& parent);

		void setAxisData(const std::vector<MovePresenter::AxisData>& axis_data);
		void setPositionType(const MovePresenter::PositionType type);
		void setCanJogUnhomed(bool enable);
		void setAxisPosition(char axis_letter, float position);
		void setAxisHomed(char axis_letter, bool homed);
		void setAxisDisabled(char axis_letter, bool disabled);
		void setAxisJogDisabled(char axis_letter, bool disabled);
		void setAxisHomeDisabled(char axis_letter, bool disabled);

		void clear();

	  private:
		static void onHomeAllEvent(lv_event_t* e);
		static void onDisableMotorsEvent(lv_event_t* e);
		static void onDistanceEvent(lv_event_t* e);
		static void onFeedrateEvent(lv_event_t* e);

		void configureNumberpadForAxis(char axis_letter, float position);

		// Axis Control
		Card m_axisControlCont{"axis_control", getRoot()};
		XYControl m_xyControl{"xy_control", m_axisControlCont};
		GenericAxisControl m_zControl{"z_control", m_axisControlCont};
		List<GenericAxisControl> m_genericAxisControls{"generic_axis_controls", m_axisControlCont};

		// Bottom Bar
		Card m_bottomBarCont{"bottombar", getRoot()};
		List<Button> m_distances{"distances", m_bottomBarCont};
		List<Button> m_feedrates{"feedrates", m_bottomBarCont};

		ModalNumberPad m_numberpad{"numberpad", getRoot()};

		const MovePresenter::AxisDataList* m_axisDataListPtr = nullptr;
	};
} // namespace UI

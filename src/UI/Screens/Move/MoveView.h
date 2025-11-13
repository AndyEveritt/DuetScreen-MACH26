#pragma once

#include "MovePresenter.h"
#include "UI/Components/AxisControl/AxisJogList.h"
#include "UI/Components/AxisControl/GenericAxisControl.h"
#include "UI/Components/AxisControl/XYControl.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/ExtruderControl/ExtruderControl.h"
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

		void setToolCount(const size_t count);
		void setToolName(const size_t index, const std::string& name);
		void setCurrentTool(const int32_t index);
		void setFilamentDisabled(bool disabled) { m_extruderControl.setFilamentDisabled(disabled); }
		void setFilamentOptions(const std::vector<std::string>& options)
		{
			m_extruderControl.setFilamentOptions(options);
		}
		void setLoadedFilament(const std::string& filament) { m_extruderControl.setFilamentSelected(filament); }

		void setExtrudeDisabled(bool disabled) { m_extruderControl.setExtrudeDisabled(disabled); }
		void setRetractDisabled(bool disabled) { m_extruderControl.setRetractDisabled(disabled); }

		void clear();

	  private:
		static void onHomeAllEvent(lv_event_t* e);
		static void onDisableMotorsEvent(lv_event_t* e);
		static void onDistanceEvent(lv_event_t* e);
		static void onFeedrateEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		void configureNumberpadForAxis(char axis_letter, float position);
		void configureNumberpadForExtruder(const std::string& header, float value);

		// Axis Control
		Card m_axisControlCont{"axis_control", getRoot()};
		XYControl m_xyControl{"xy_control", m_axisControlCont};
		GenericAxisControl m_zControl{"z_control", m_axisControlCont};
		List<GenericAxisControl> m_genericAxisControls{"generic_axis_controls", m_axisControlCont};

		// Extruder Control
		ExtruderControl m_extruderControl{"extruder_control", m_axisControlCont};

		// Bottom Bar
		Card m_bottomBarCont{"bottombar", getRoot()};
		List<Button> m_distances{"distances", m_bottomBarCont};
		List<Button> m_feedrates{"feedrates", m_bottomBarCont};

		ModalNumberPad m_numberpad{"numberpad", getRoot()};

		const MovePresenter::AxisDataList* m_axisDataListPtr = nullptr;
	};
} // namespace UI

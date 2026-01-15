/*
 * PrintInfo.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "PrintInfoPresenter.h"
#include "UI/Components/Bar/Bar.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/List/List.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Core/View.h"
#include "UI/Widgets/BabyStep/BabyStep.h"
#include "UI/Widgets/Control/ExtrusionFactor/ExtrusionFactor.h"
#include "UI/Widgets/Control/SpeedFactor/SpeedFactor.h"

namespace UI
{
	class PrintInfo : public View<PrintInfoPresenter>
	{
	  public:
		PrintInfo(const std::string& name, LvObj& parent);

		void setAxisCount(size_t count);
		void setPosition(size_t index, char axis_letter, float value);
		void setMaxSpeed(int32_t max_speed);
		void setMaxExtrusionRate(int32_t max_extrusion_rate);
		void updateExtrusionRate(float feedrate, float volumetric);
		void updateSpeed(float topSpeed, float requestedSpeed);
		void updateFlowMultiplier(uint32_t multiplier);
		void updateSpeedMultiplier(uint32_t multiplier);
		void updateElapsedTime(uint32_t elapsed);
		void updateRemainingTime(uint32_t remaining);
		void updateBabyStep(float babystep);

		auto& getModalExtrusionFactor() { return m_extrusionFactorModal; }

		void setNumberPad(ModalNumberPad* numberPad);

	  private:
		static void openSubView(lv_event_t* e);

		void onInit() override;
		void onShow() override;
		void onHide() override;

		List<LvLabel> m_positions{"positions", getRoot()};

		LvContainer m_speedCont{"speed_cont", getRoot()};
		LvLabel m_speedHeader{"speed_header", m_speedCont};
		Button m_speedMultiplier{"speed_multiplier", m_speedCont};
		LvBar m_requestedSpeed{"requested_speed", m_speedCont};
		LvBar m_currentSpeed{"current_speed", m_speedCont};
		LvLabel m_speedLabel{"speed_label", m_currentSpeed};

		LvContainer m_flowCont{"flow_cont", getRoot()};
		LvLabel m_flowHeader{"flow_header", m_flowCont};
		Button m_flowMultiplier{"flow_multiplier", m_flowCont};
		LvBar m_extruderFeedrate{"extruder_feedrate", m_flowCont};
		LvLabel m_extruderFeedrateLabel{"extruder_feedrate_label", m_extruderFeedrate};
		// LvLabel m_flowRateLabel{"flow_rate", m_flowCont};

		LvContainer m_timeCont{"time_cont", getRoot()};
		LvLabel m_timeHeader{"time_header", m_timeCont};
		LvLabel m_elapsedTime{"elapsed_time", m_timeCont};
		LvLabel m_remainingTime{"remaining_time", m_timeCont};

		LvContainer m_babyStepCont{"babystep_cont", getRoot()};
		LvLabel m_babyStepHeader{"babystep_header", m_babyStepCont};
		Button m_babyStepButton{"babystep_button", m_babyStepCont};

		Modal<SpeedFactor> m_speedFactorModal{"speed_factor", getRoot()};
		ModalExtrusionFactor m_extrusionFactorModal{"extrusion_factor", getRoot()};
		Modal<BabyStep> m_babyStepModal{"baby_step", getRoot()};
	};
} // namespace UI

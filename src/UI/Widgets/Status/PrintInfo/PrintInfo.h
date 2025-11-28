/*
 * PrintInfo.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "PrintInfoPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/List/List.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Widgets/Control/ExtrusionFactor/ExtrusionFactor.h"
#include "UI/Core/View.h"
#include "UI/Widgets/BabyStep/BabyStep.h"
#include "UI/Widgets/Control/SpeedFactor/SpeedFactor.h"

namespace UI
{
	class PrintInfo : public View<PrintInfoPresenter>
	{
	  public:
		PrintInfo(const std::string& name, LvObj& parent);

		void setAxisCount(size_t count);
		void setPosition(size_t index, char axis_letter, float value);
		void updateExtrusionRate(float feedrate, float volumetric);
		void updateSpeed(float topSpeed, float requestedSpeed);
		void updateFlowMultiplier(uint32_t multiplier);
		void updateSpeedMultiplier(uint32_t multiplier);
		void updateElapsedTime(uint32_t elapsed);
		void updateRemainingTime(uint32_t remaining);

		auto& getModalExtrusionFactor() { return m_extrusionFactorModal; }

	  private:
		static void openSubView(lv_event_t* e);

		void onShow() override;
		void onHide() override;

		List<LvLabel> m_positions{"positions", getRoot()};

		LvContainer m_speedCont{"speed_cont", getRoot()};
		LvLabel m_speedHeader{"speed_header", m_speedCont};
		LvLabel m_currentSpeed{"current_speed", m_speedCont};
		LvLabel m_requestedSpeed{"top_speed", m_speedCont};
		Button m_speedMultiplier{"speed_multiplier", m_speedCont};

		LvContainer m_flowCont{"flow_cont", getRoot()};
		LvLabel m_flowHeader{"flow_header", m_flowCont};
		LvLabel m_extruderFeedrate{"extruder_feedrate", m_flowCont};
		LvLabel m_flowRate{"flow_rate", m_flowCont};
		Button m_flowMultiplier{"flow_multiplier", m_flowCont};

		LvContainer m_timeCont{"time_cont", getRoot()};
		LvLabel m_elapsedTime{"elapsed_time", m_timeCont};
		LvLabel m_remainingTime{"remaining_time", m_timeCont};

		BabyStep m_babyStep{"baby_step", getRoot()};

		Modal<SpeedFactor> m_speedFactorModal{"speed_factor", getRoot()};
		ModalExtrusionFactor m_extrusionFactorModal{"extrusion_factor", getRoot()};

		bool m_initialised = false;
	};
} // namespace UI

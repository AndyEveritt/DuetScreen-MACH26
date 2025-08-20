/*
 * PrintInfo.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Core/View.h"

namespace UI
{
	class PrintInfo : public LvContainer
	{
	  public:
		PrintInfo(lv_obj_t* parent);

		void updateToolTemp(float temp, int32_t target);
		void updateBedTemp(float temp, int32_t target);
		void updateExtrusionRate(float feedrate, float volumetric);
		void updateSpeed(float topSpeed, float requestedSpeed);
		void updateFlowMultiplier(uint32_t multiplier);
		void updateSpeedMultiplier(uint32_t multiplier);
		void updateElapsedTime(uint32_t elapsed);
		void updateRemainingTime(uint32_t remaining);
		void updateLayer(float height, float maxHeight);
		void updateFanSpeed(uint32_t speed);
		void updateAcceleration(uint32_t acceleration);
		void updatePosition(float x, float y, float z);
		void updateZOffset(float offset);
		void updateLayerNumber(uint32_t layer);

		virtual bool back() override;

	  private:
		class SpeedInfo : public LvObj
		{
		  public:
			SpeedInfo(lv_obj_t* parent);

			void updateSpeed(float topSpeed, float requestedSpeed);
			void updateSpeedMultiplier(uint32_t multiplier);
			void updateAcceleration(uint32_t acceleration);
			void updatePosition(float x, float y, float z);
			void updateZOffset(float offset);
			void updatePrintHeight(float height);
			void updateLayerNumber(uint32_t layer);

		  private:
			LvLabel m_speed;
			LvLabel m_speedMultiplier;
			LvLabel m_acceleration;
			LvLabel m_position;
			LvLabel m_z_offset;
			LvLabel m_z_height;
			LvLabel m_layer;
		};

		static void openExtrudeView(lv_event_t* e);
		static void openSubView(lv_event_t* e);

		LvLabel m_speed;
		LvLabel m_speedMultiplier;
		LvLabel m_flowRate;
		LvLabel m_flowMultiplier;
		LvLabel m_elapsedTime;
		LvLabel m_remainingTime;
		LvLabel m_layer;
		LvLabel m_fanSpeed;

		SpeedInfo m_speedInfo;
	};
} // namespace UI

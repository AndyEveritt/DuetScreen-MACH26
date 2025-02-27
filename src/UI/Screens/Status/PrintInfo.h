/*
 * PrintInfo.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/View.h"

namespace UI
{
	class PrintInfo : public BaseView
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

		virtual bool back() override;

	  private:
		class SpeedInfo : public BaseView
		{
		  public:
			SpeedInfo(lv_obj_t* parent);

			void updateSpeed(float topSpeed, float requestedSpeed);
			void updateSpeedMultiplier(uint32_t multiplier);
			void updateAcceleration(uint32_t acceleration);
			void updatePosition(float x, float y, float z);
			void updateZOffset(float offset);
			void updateZHeight(float height);
			void updateLayer(uint32_t layer);

		  private:
			lv_obj_t* m_speed;
			lv_obj_t* m_speedMultiplier;
			lv_obj_t* m_acceleration;
			lv_obj_t* m_position;
			lv_obj_t* m_z_offset;
			lv_obj_t* m_z_height;
			lv_obj_t* m_layer;
		};

		static void openExtrudeView(lv_event_t* e);
		static void openSubView(lv_event_t* e);

		lv_obj_t* m_toolTemp;
		lv_obj_t* m_bedTemp;
		lv_obj_t* m_speed;
		lv_obj_t* m_speedMultiplier;
		lv_obj_t* m_flowRate;
		lv_obj_t* m_flowMultiplier;
		lv_obj_t* m_elapsedTime;
		lv_obj_t* m_remainingTime;
		lv_obj_t* m_layer;
		lv_obj_t* m_fanSpeed;

		SpeedInfo m_speedInfo;
	};
} // namespace UI

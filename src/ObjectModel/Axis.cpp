/*
 * Axis.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#include "Axis.h"

#include "Configuration.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Tool.h"
#include <Duet3D/General/Vector.h>
#include <math.h>

#include "Debug.h"
#include "ListHelpers.h"

namespace OM::Move
{
	typedef Vector<std::shared_ptr<Axis>, MAX_TOTAL_AXES> AxisList;
	typedef Vector<std::shared_ptr<ExtruderAxis>, MAX_TOTAL_AXES> ExtruderAxisList;
	static AxisList s_axes;
	static ExtruderAxisList s_extruderAxes;
	static float s_extrusionRate = 0.0f;
	static float s_speedFactor = 100.0f;
	static float s_currentMoveRequestedSpeed = 0.0f;
	static float s_currentMoveTopSpeed = 0.0f;
	static uint8_t s_currentWorkplaceNumber = Workplaces::MaxTotalWorkplaces;
	static uint32_t s_printingAcceleration = 0;
	static Kinematics s_kinematics;

	void Axis::Reset()
	{
		index = 0;
		babystep = 0.0f;
		letter[0] = 0;
		letter[1] = 0;
		for (size_t i = 0; i < Workplaces::MaxTotalWorkplaces; ++i)
		{
			workplaceOffsets[i] = 0.0f;
		}
		homed = false;
		visible = false;
	}

	void Axis::Home()
	{
		Comm::DUET.SendGcodef("G28 %s\n", letter);
	}

	void Axis::MoveRelative(float distance, uint32_t feedrate)
	{
		feedrate = feedrate * 60;
		Comm::DUET.SendGcodef("G91\nG1 %s%.2f F%u\nG90\n", letter, distance, feedrate);
	}

	std::shared_ptr<Axis> GetAxis(const size_t index)
	{
		LOG_DBG("Axis index {:d} / max {:d}", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return GetOrCreate<AxisList, Axis>(s_axes, index, true);
	}

	std::shared_ptr<Axis> GetAxisBySlot(const size_t slot, const bool includeHidden)
	{
		LOG_DBG("Slot {:d}{:s}", slot, includeHidden ? " (including hidden)" : "");
		size_t count = 0;
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			auto axis = GetAxis(i);
			if (axis == nullptr)
			{
				continue;
			}
			if (!includeHidden && !axis->visible)
			{
				continue;
			}
			if (count == slot)
			{
				return axis;
			}
			count++;
		}
		return nullptr;
	}

	std::shared_ptr<Axis> GetAxisByLetter(const char letter)
	{
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			auto axis = GetAxis(i);
			if (axis != nullptr && axis->letter[0] == letter)
			{
				return axis;
			}
		}
		return nullptr;
	}

	std::shared_ptr<Axis> GetOrCreateAxis(const size_t index)
	{
		LOG_DBG("Axis index {:d} / max {:d}", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return GetOrCreate<AxisList, Axis>(s_axes, index, true);
	}

	size_t GetAxisCount(const bool includeHidden)
	{
		size_t count = 0;
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			auto axis = GetAxis(i);
			if (axis == nullptr)
			{
				continue;
			}
			if (!includeHidden && !axis->visible)
			{
				continue;
			}
			count++;
		}
		return count;
	}

	bool IterateAxesWhile(function_ref<bool(std::shared_ptr<Axis>, size_t)> func, const size_t startAt)
	{
		return s_axes.IterateWhile(func, startAt);
	}

	size_t RemoveAxis(const size_t index, const bool allFollowing)
	{
		return Remove<AxisList, Axis>(s_axes, index, allFollowing);
	}

#define AXIS_SETTER(funcName, valType, varName)                                                                        \
  bool funcName(size_t index, valType val)                                                                             \
  {                                                                                                                    \
	if (index >= MAX_TOTAL_AXES)                                                                                       \
	{                                                                                                                  \
	  LOG_ERROR("axis[{:d}] greater than MAX_TOTAL_AXES", index);                                                      \
	  return false;                                                                                                    \
	}                                                                                                                  \
	std::shared_ptr<Axis> axis = GetOrCreateAxis(index);                                                               \
	if (axis == nullptr)                                                                                               \
	{                                                                                                                  \
	  LOG_ERROR("Could not get or create axis {:d}", index);                                                           \
	  return false;                                                                                                    \
	}                                                                                                                  \
	axis->varName = val;                                                                                               \
	return true;                                                                                                       \
  }

	AXIS_SETTER(SetAcceleration, uint32_t, acceleration);
	AXIS_SETTER(SetBabystepOffset, float, babystep);
	// Update the homed status of the specified axis. If the axis is -1 then it
	// represents the "all homed" status.
	AXIS_SETTER(SetAxisHomedStatus, bool, homed);
	AXIS_SETTER(SetAxisLetter, char, letter[0]);
	AXIS_SETTER(SetAxisUserPosition, float, userPosition);
	AXIS_SETTER(SetAxisMachinePosition, float, machinePosition);
	AXIS_SETTER(SetAxisMinPosition, float, minPosition);
	AXIS_SETTER(SetAxisMaxPosition, float, maxPosition);
	AXIS_SETTER(SetAxisVisible, bool, visible);

	bool SetAxisWorkplaceOffset(size_t axisIndex, size_t workplaceIndex, float offset)
	{
		if (axisIndex >= MAX_TOTAL_AXES || workplaceIndex >= OM::Move::Workplaces::MaxTotalWorkplaces)
			return false;
		auto axis = GetOrCreateAxis(axisIndex);
		if (axis == nullptr)
			return false;
		axis->workplaceOffsets[workplaceIndex] = offset;
		return true;
	}

	bool SetCurrentWorkplaceNumber(uint8_t workplaceNumber)
	{
		if (workplaceNumber >= Workplaces::MaxTotalWorkplaces)
		{
			return false;
		}
		s_currentWorkplaceNumber = workplaceNumber;
		return true;
	}

	const uint8_t GetCurrentWorkplaceNumber()
	{
		return s_currentWorkplaceNumber;
	}

	void SetPrintingAcceleration(uint32_t acceleration)
	{
		s_printingAcceleration = acceleration;
	}

	const uint32_t& GetPrintingAcceleration()
	{
		return s_printingAcceleration;
	}

	void ExtruderAxis::Reset()
	{
		index = 0;
		position = 0.0f;
		factor = 0.0f;
		stepsPerMm = 0.0f;
	}

	void Reset()
	{
		RemoveAxis(0, true);
		RemoveExtruderAxis(0, true);
		s_currentWorkplaceNumber = OM::Move::Workplaces::MaxTotalWorkplaces;
		s_kinematics.Reset();
		SetSpeedFactor(100.0f);
		SetCurrentMoveRequestedSpeed(0.0f);
		SetCurrentMoveTopSpeed(0.0f);
		SetExtrusionRate(0.0f);
		SetPrintingAcceleration(0);
	}

	std::shared_ptr<ExtruderAxis> GetExtruderAxis(const size_t index)
	{
		LOG_DBG("ExtruderAxis index {:d} / max {:d}", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return GetOrCreate<ExtruderAxisList, ExtruderAxis>(s_extruderAxes, index, false);
	}

	std::shared_ptr<ExtruderAxis> GetExtruderAxisBySlot(const size_t slot)
	{
		if (slot >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return s_extruderAxes[slot];
	}

	std::shared_ptr<ExtruderAxis> GetOrCreateExtruderAxis(const size_t index)
	{
		LOG_DBG("ExtruderAxis index {:d} / max {:d}", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			LOG_ERROR("ExtruderAxis index {:d} greater than MAX_TOTAL_AXES", index);
			return nullptr;
		}
		return GetOrCreate<ExtruderAxisList, ExtruderAxis>(s_extruderAxes, index, true);
	}

	size_t GetExtruderAxisCount()
	{
		return s_extruderAxes.Size();
	}

	bool IterateExtruderAxesWhile(function_ref<bool(std::shared_ptr<ExtruderAxis>, size_t)> func, const size_t startAt)
	{
		return s_extruderAxes.IterateWhile(func, startAt);
	}

	size_t RemoveExtruderAxis(const size_t index, const bool allFollowing)
	{
		return Remove<ExtruderAxisList, ExtruderAxis>(s_extruderAxes, index, allFollowing);
	}

#define EXTRUDER_AXIS_SETTER(funcName, valType, varName)                                                               \
  bool funcName(size_t index, valType val)                                                                             \
  {                                                                                                                    \
	std::shared_ptr<ExtruderAxis> extruder = GetOrCreateExtruderAxis(index);                                           \
	if (extruder == nullptr)                                                                                           \
	{                                                                                                                  \
	  LOG_ERROR("Could not get or create extruderAxis {:d}", index);                                                   \
	  return false;                                                                                                    \
	}                                                                                                                  \
	extruder->varName = val;                                                                                           \
	return true;                                                                                                       \
  }

	EXTRUDER_AXIS_SETTER(SetExtruderPosition, float, position);
	EXTRUDER_AXIS_SETTER(SetExtruderFactor, float, factor);
	EXTRUDER_AXIS_SETTER(SetExtruderStepsPerMm, float, stepsPerMm);
	EXTRUDER_AXIS_SETTER(SetExtruderFilamentDiameter, float, filamentDiameter);
	EXTRUDER_AXIS_SETTER(SetExtruderPressureAdvance, float, pressureAdvance);

	bool SetExtruderFilamentName(size_t index, const char* name)
	{
		auto extruder = GetOrCreateExtruderAxis(index);
		if (extruder == nullptr)
		{
			LOG_ERROR("Could not get or create extruderAxis {:d}", index);
			return false;
		}
		extruder->filamentName.CopyAndPad(name);
		return true;
	}

	void SetExtrusionRate(float rate)
	{
		s_extrusionRate = rate;
	}

	const float GetExtrusionRate()
	{
		return s_extrusionRate;
	}

	const float GetVolumetricFlow()
	{
		auto tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			return 0.0f;
		}
		float filamentArea = 0;
		float numExtruders = 0;
		tool->IterateExtruders(
			[&](std::shared_ptr<OM::Move::ExtruderAxis> extruder, size_t index)
			{
				if (extruder == nullptr)
					return;
				numExtruders++;
				filamentArea += tool->mix[index] * (M_PI * pow(extruder->filamentDiameter / 2, 2));
			});

		if (numExtruders == 0)
		{
			return 0.0f;
		}

		filamentArea /= numExtruders;
		return filamentArea * GetExtrusionRate();
	}

	const float GetSpeedFactor()
	{
		return s_speedFactor;
	}

	void SetSpeedFactor(float factor)
	{
		s_speedFactor = factor;
	}

	const float GetCurrentMoveRequestedSpeed()
	{
		return s_currentMoveRequestedSpeed;
	}

	void SetCurrentMoveRequestedSpeed(float speed)
	{
		s_currentMoveRequestedSpeed = speed;
	}

	const float GetCurrentMoveTopSpeed()
	{
		return s_currentMoveTopSpeed;
	}

	void SetCurrentMoveTopSpeed(float speed)
	{
		s_currentMoveTopSpeed = speed;
	}

	void SetKinematicsName(const std::string& name)
	{
		s_kinematics.name = name;
	}

	const Kinematics& GetKinematics()
	{
		return s_kinematics;
	}
} // namespace OM::Move

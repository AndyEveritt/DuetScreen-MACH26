#pragma once

#include "Subscribers/Subscribers.h"

class MoveSubscribers : public SubscriberMap
{
  public:
	MoveSubscribers()
	{
		addSubscriber("move:axes^:acceleration", acceleration);
		addSubscriber("move:axes^:babystep", babyStep);
		addSubscriber("move:axes^:homed", axisHomed);
		addSubscriber("move:axes^:letter", axisLetter);
		addSubscriber("move:axes^:machinePosition", axisMachinePosition);
		addSubscriber("move:axes^:min", axisMinPosition);
		addSubscriber("move:axes^:max", axisMaxPosition);
		addSubscriber("move:axes^:userPosition", axisUserPosition);
		addSubscriber("move:axes^:speed", axisMaxSpeed);
		addSubscriber("move:axes^:visible", axisVisible);
		addSubscriber("move:axes^:workplaceOffsets^", axisWorkplaceOffset);
		// addSubscriber("move:axes:next", axisNext);
		addSubscriber("move:extruders^:factor", extrusionFactor);
		addSubscriber("move:extruders^:filamentDiameter", extruderFilamentDiameter);
		addSubscriber("move:extruders^:filament", extruderFilamentName);
		addSubscriber("move:extruders^:position", extruderPosition);
		addSubscriber("move:extruders^:pressureAdvance", extruderPressureAdvance);
		addSubscriber("move:extruders^:speed", extruderMaxSpeed);
		addSubscriber("move:extruders^:stepsPerMm", extruderStepsPerMm);
		addSubscriber("move:kinematics:name", kinematicsName);
		addSubscriber("move:speedFactor", speedFactor);
		addSubscriber("move:workplaceNumber", workplaceNumber);
		addSubscriber("move:noMovesBeforeHoming", noMovesBeforeHoming);
		addSubscriber("move:printingAcceleration", printingAcceleration);
		addSubscriber("move:currentMove:requestedSpeed", currentMoveRequestedSpeed);
		addSubscriber("move:currentMove:topSpeed", currentMoveTopSpeed);
		addSubscriber("move:currentMove:extrusionRate", currentMoveExtrusionRate);
		addSubscriber("move:compensation:file", compensationFile);
		addSubscriber("inputs^:distanceUnit", distanceUnit);

		addArrayEndSubscriber("move:axes^", axesArrayEnd);
		addArrayEndSubscriber("move:extruders^", extrudersArrayEnd);
	}

  private:
	static bool acceleration(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool babyStep(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool axisHomed(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[]);
	static bool axisLetter(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool axisMachinePosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool axisMinPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool axisMaxPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool axisUserPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool axisMaxSpeed(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool axisVisible(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[]);
	static bool axisWorkplaceOffset(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool axisNext(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool extrusionFactor(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool extruderFilamentDiameter(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool extruderFilamentName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool extruderPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool extruderPressureAdvance(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool extruderMaxSpeed(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[]);
	static bool extruderStepsPerMm(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool kinematicsName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool speedFactor(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool workplaceNumber(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool noMovesBeforeHoming(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[]);
	static bool printingAcceleration(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[]);
	static bool currentMoveRequestedSpeed(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool currentMoveTopSpeed(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool currentMoveExtrusionRate(Comm::JsonDecoder* decoder, const float& data, const size_t indices[]);
	static bool compensationFile(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	static bool distanceUnit(Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);

	static bool axesArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
	static bool extrudersArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[]);
};

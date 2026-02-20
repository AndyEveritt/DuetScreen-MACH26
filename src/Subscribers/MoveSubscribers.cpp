#include "Debug.h"

#include "Hardware/Duet.h"
#include "MoveSubscribers.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Heightmap.h"
#include "UI/Core/Model.h"

bool MoveSubscribers::acceleration(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAcceleration(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->babystep = {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::babyStep(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetBabystepOffset(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->babystep = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisHomed(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisHomedStatus(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->homed = {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisLetter(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisLetter(indices[0], data[0]))
	{
		LOG_ERROR("Failed to set axis[{:d}]->letter = {:s}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisMachinePosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisMachinePosition(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->machinePosition = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisMinPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisMinPosition(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->min = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisMaxPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisMaxPosition(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->max = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisUserPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisUserPosition(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->userPosition = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisMaxSpeed(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisMaxSpeed(indices[0], data / 60))
	{
		LOG_ERROR("Failed to set axis[{:d}]->maxSpeed = {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisVisible(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisVisible(indices[0], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->visible = {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisWorkplaceOffset(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetAxisWorkplaceOffset(indices[0], indices[1], data))
	{
		LOG_ERROR("Failed to set axis[{:d}]->workplaceOffset[{:d}] = {:g}", indices[0], indices[1], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extrusionFactor(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetExtruderFactor(indices[0], data))
	{
		LOG_ERROR("Failed to set extruderAxis[{:d}]->factor = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderFilamentDiameter(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetExtruderFilamentDiameter(indices[0], data))
	{
		LOG_ERROR("Failed to set extruderAxis[{:d}]->filamentDiameter = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderFilamentName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetExtruderFilamentName(indices[0], data))
	{
		LOG_ERROR("Failed to set extruderAxis[{:d}]->filamentName = {:s}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetExtruderPosition(indices[0], data))
	{
		LOG_ERROR("Failed to set extruderAxis[{:d}]->position = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderPressureAdvance(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetExtruderPressureAdvance(indices[0], data))
	{
		LOG_ERROR("Failed to set extruderAxis[{:d}]->pressureAdvance = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderMaxSpeed(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetExtruderMaxSpeed(indices[0], data / 60))
	{
		LOG_ERROR("Failed to set extruderAxis[{:d}]->maxSpeed = {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderStepsPerMm(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Move::SetExtruderStepsPerMm(indices[0], data))
	{
		LOG_ERROR("Failed to set extruderAxis[{:d}]->stepsPerMm = {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::kinematicsName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Move::SetKinematicsName(data);
	Model::get().post<EventType::KinematicsName>(OM::Move::GetKinematics().name);
	return true;
}

bool MoveSubscribers::speedFactor(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Move::SetSpeedFactor(data);
	Model::get().post<EventType::SpeedFactor>();
	return true;
}

bool MoveSubscribers::workplaceNumber(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	if (!OM::Move::SetCurrentWorkplaceNumber((uint8_t)data))
	{
		LOG_ERROR("Failed to set workplace number = {:d}", data);
		return false;
	}
	Model::get().post<EventType::WorkplaceNumber>();
	return true;
}

bool MoveSubscribers::noMovesBeforeHoming(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Move::SetNoMovesBeforeHoming(data);
	Model::get().post<EventType::NoMoveBeforeHoming>();
	return true;
}

bool MoveSubscribers::printingAcceleration(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Move::SetPrintingAcceleration(data);
	Model::get().post<EventType::PrintingAcceleration>(OM::Move::GetPrintingAcceleration());
	return true;
}

bool MoveSubscribers::currentMoveRequestedSpeed(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Move::SetCurrentMoveRequestedSpeed(data);
	Model::get().post<EventType::CurrentMoveRequestedSpeed>();
	return true;
}

bool MoveSubscribers::currentMoveTopSpeed(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Move::SetCurrentMoveTopSpeed(data);
	Model::get().post<EventType::CurrentMoveTopSpeed>();
	return true;
}

bool MoveSubscribers::currentMoveExtrusionRate(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Move::SetExtrusionRate(data);
	Model::get().post<EventType::CurrentMoveExtrusionSpeed>();
	return true;
}

bool MoveSubscribers::compensationFile(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::SetCurrentHeightmap(data);
	Model::get().post<EventType::CompensationFile>();
	return true;
}

bool MoveSubscribers::distanceUnit(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	LOG_DBG("New distance unit: {:s}, channel: {:d}", data, indices[0]);
	if (strcmp(data, "mm") == 0)
	{
		OM::Move::SetDistanceUnit(indices[0], Units::UnitSystem::Metric);
	}
	else if (strcmp(data, "in") == 0)
	{
		OM::Move::SetDistanceUnit(indices[0], Units::UnitSystem::Imperial);
	}
	else
	{
		LOG_ERROR("Unknown distance unit: {:s}", data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axesArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	const Comm::Seq* seq = decoder->GetSeq();
	if (seq && seq->seqid == Comm::rcvSeqsMove && indices[0] >= MAX_REPORTED_AXES)
	{
		/**
		 * There might be more axes available
		 *
		 * TODO: this won't handle the case where the single `move.axes` request still doesn't return all axes but it
		 * can handle 10 currently. Will need to check the `next` field in the response to handle more than that.
		 * Likely I will postpone handling that until the Duet comms are reworked to handle request/response IDs (#69)
		 */
		Comm::DUET.RequestModel("move.axes", "vna0");
		return true;
	}
	OM::Move::RemoveAxis(indices[0], true);
	Model::get().post<EventType::AxesData>();
	return true;
}

bool MoveSubscribers::extrudersArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	OM::Move::RemoveExtruderAxis(indices[0], true);
	Model::get().post<EventType::ExtruderData>();
	return true;
}

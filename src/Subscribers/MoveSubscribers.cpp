#include "Debug.h"

#include "MoveSubscribers.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Heightmap.h"
#include "UI/Core/Model.h"

bool MoveSubscribers::babyStep(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetBabystepOffset(indices[0], data))
	{
		error("Failed to set axis[%d]->babystep = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisHomed(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	if (!OM::Move::SetAxisHomedStatus(indices[0], data))
	{
		error("Failed to set axis[%d]->homed = %d", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisLetter(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	if (!OM::Move::SetAxisLetter(indices[0], data[0]))
	{
		error("Failed to set axis[%d]->letter = %s", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisMachinePosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetAxisMachinePosition(indices[0], data))
	{
		error("Failed to set axis[%d]->machinePosition = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisMinPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetAxisMinPosition(indices[0], data))
	{
		error("Failed to set axis[%d]->min = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisMaxPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetAxisMaxPosition(indices[0], data))
	{
		error("Failed to set axis[%d]->max = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisUserPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetAxisUserPosition(indices[0], data))
	{
		error("Failed to set axis[%d]->userPosition = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisVisible(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	if (!OM::Move::SetAxisVisible(indices[0], data))
	{
		error("Failed to set axis[%d]->visible = %d", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::axisWorkplaceOffset(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetAxisWorkplaceOffset(indices[0], indices[1], data))
	{
		error("Failed to set axis[%d]->workplaceOffset[%d] = %f", indices[0], indices[1], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extrusionFactor(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetExtruderFactor(indices[0], data))
	{
		error("Failed to set extruderAxis[%d]->factor = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderFilamentDiameter(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetExtruderFilamentDiameter(indices[0], data))
	{
		error("Failed to set extruderAxis[%d]->filamentDiameter = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderFilamentName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	if (!OM::Move::SetExtruderFilamentName(indices[0], data))
	{
		error("Failed to set extruderAxis[%d]->filamentName = %s", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderPosition(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetExtruderPosition(indices[0], data))
	{
		error("Failed to set extruderAxis[%d]->position = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderPressureAdvance(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetExtruderPressureAdvance(indices[0], data))
	{
		error("Failed to set extruderAxis[%d]->pressureAdvance = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::extruderStepsPerMm(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Move::SetExtruderStepsPerMm(indices[0], data))
	{
		error("Failed to set extruderAxis[%d]->stepsPerMm = %f", indices[0], data);
		return false;
	}
	return true;
}

bool MoveSubscribers::kinematicsName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	Model::get().newKinematicsName();
	return true;
}

bool MoveSubscribers::speedFactor(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	OM::Move::SetSpeedFactor(data);
	Model::get().newSpeedFactor();
	return true;
}

bool MoveSubscribers::workplaceNumber(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::Move::SetCurrentWorkplaceNumber((uint8_t)data))
	{
		error("Failed to set workplace number = %d", data);
		return false;
	}
	Model::get().newWorkplaceNumber();
	return true;
}

bool MoveSubscribers::currentMoveRequestedSpeed(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	OM::Move::SetCurrentMoveRequestedSpeed(data);
	Model::get().newCurrentMoveRequestedSpeed();
	return true;
}

bool MoveSubscribers::currentMoveTopSpeed(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	OM::Move::SetCurrentMoveTopSpeed(data);
	Model::get().newCurrentMoveTopSpeed();
	return true;
}

bool MoveSubscribers::currentMoveExtrusionRate(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	OM::Move::SetExtrusionRate(data);
	Model::get().newCurrentMoveExtrusionSpeed();
	return true;
}

bool MoveSubscribers::compensationFile(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::SetCurrentHeightmap(data);
	Model::get().newCompensationFile();
	return true;
}

bool MoveSubscribers::axesArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	OM::Move::RemoveAxis(indices[0], true);
	Model::get().newAxesData();
	return true;
}

bool MoveSubscribers::extrudersArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	OM::Move::RemoveExtruderAxis(indices[0], true);
	Model::get().newExtruderData();
	return true;
}

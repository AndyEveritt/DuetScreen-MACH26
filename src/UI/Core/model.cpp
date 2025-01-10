#include "model.h"
#include "presenter.h"

#define NOTIFY_ALL_PRESENTERS(func)                                                                                    \
	for (auto presenter : m_presenters)                                                                                \
	{                                                                                                                  \
		presenter->func();                                                                                             \
	}

#define MODEL_NOTIFICATION(func)                                                                                       \
	void Model::func()                                                                                                 \
	{                                                                                                                  \
		NOTIFY_ALL_PRESENTERS(func);                                                                                   \
	}

MODEL_NOTIFICATION(newFanData)

/* File methods */

MODEL_NOTIFICATION(newFileData)

/* Heater methods */

MODEL_NOTIFICATION(newHeaterData)

/* Job methods */

MODEL_NOTIFICATION(newJobFileName)
MODEL_NOTIFICATION(newJobLastFileName)
MODEL_NOTIFICATION(newJobPrintTime)
MODEL_NOTIFICATION(newJobDuration)
MODEL_NOTIFICATION(newJobTimeLeft)
MODEL_NOTIFICATION(newJobWarmupDuration)
MODEL_NOTIFICATION(newJobBuild)
MODEL_NOTIFICATION(newJobCurrentObject)
MODEL_NOTIFICATION(newJobObjectData)

/* Move methods */

MODEL_NOTIFICATION(newAxesData)
MODEL_NOTIFICATION(newExtruderData)
MODEL_NOTIFICATION(newKinematicsName)
MODEL_NOTIFICATION(newSpeedFactor)
MODEL_NOTIFICATION(newWorkplaceNumber)
MODEL_NOTIFICATION(newCurrentMoveRequestedSpeed)
MODEL_NOTIFICATION(newCurrentMoveTopSpeed)
MODEL_NOTIFICATION(newCurrentMoveExtrusionSpeed)
MODEL_NOTIFICATION(newCompensationFile)

/* Response methods */

MODEL_NOTIFICATION(newResponse)

/* Sensor methods */

MODEL_NOTIFICATION(newAnalogSensorData)
MODEL_NOTIFICATION(newEndstopData)

/* Spindle methods */

MODEL_NOTIFICATION(newSpindleData)

/* State methods */

MODEL_NOTIFICATION(newNetworkName)
MODEL_NOTIFICATION(newIpAddress)
MODEL_NOTIFICATION(newStatus)
MODEL_NOTIFICATION(newCurrentTool)
MODEL_NOTIFICATION(newMessageBoxData)
MODEL_NOTIFICATION(newTime)

/* Tool methods */

MODEL_NOTIFICATION(newToolData)

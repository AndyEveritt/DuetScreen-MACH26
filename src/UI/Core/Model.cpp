#include "Model.h"
#include "Debug.h"
#include "Presenter.h"

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

void Model::runSubscribers(const char* key, Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto subscribers = getSubscribers(key);
	if (subscribers.size() != 0)
	{
		dbg("found %d subscribers for '%s'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			subscriber.run(decoder, data, indices);
		}
	}
}

void Model::runArrayEndSubscribers(const char* key, Comm::JsonDecoder* decoder, const size_t indices[])
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto subscribers = getArrayEndSubscribers(key);
	if (subscribers.size() != 0)
	{
		dbg("found %d array end subscribers for '%s'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			subscriber.run(decoder, indices);
		}
	}
}

void Model::lock()
{
	dbg("Attempting to lock model");
	m_mutex.lock();
}

void Model::unlock()
{
	dbg("Unlocking model");
	m_mutex.unlock();
}

void Model::refresh()
{
	for (auto presenter : m_presenters)
	{
		presenter->newFanData();
		presenter->newFileData();
		presenter->newHeaterData();
		presenter->newJobFileName();
		presenter->newJobLastFileName();
		presenter->newJobPrintTime();
		presenter->newJobDuration();
		presenter->newJobTimeLeft();
		presenter->newJobWarmupDuration();
		presenter->newJobBuild();
		presenter->newJobCurrentObject();
		presenter->newJobObjectData();
		presenter->newAxesData();
		presenter->newExtruderData();
		presenter->newKinematicsName();
		presenter->newSpeedFactor();
		presenter->newWorkplaceNumber();
		presenter->newCurrentMoveRequestedSpeed();
		presenter->newCurrentMoveTopSpeed();
		presenter->newCurrentMoveExtrusionSpeed();
		presenter->newCompensationFile();
		presenter->newResponse();
		presenter->newAnalogSensorData();
		presenter->newEndstopData();
		presenter->newSpindleData();
		presenter->newNetworkName();
		presenter->newIpAddress();
		presenter->newStatus();
		presenter->newCurrentTool();
		presenter->newMessageBoxData();
		presenter->newTime();
		presenter->newToolData();
	}
}

/* Fan methods */

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

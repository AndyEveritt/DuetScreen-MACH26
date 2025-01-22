#include "Model.h"
#include "Comm/JsonDecoder.h"
#include "Comm/Usb.h"
#include "Debug.h"
#include "Hardware/Duet.h"
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

Model::Model()
{
	// Timers
	m_timers.tick =
		lv_timer_create([](lv_timer_t* timer) { static_cast<Model*>(lv_timer_get_user_data(timer))->tick(); },
						MODEL_TICK_INTERVAL,
						this);

#if !MULTITHREADED
	m_timers.request =
		lv_timer_create([](lv_timer_t* timer) { static_cast<Model*>(lv_timer_get_user_data(timer))->requestNewData(); },
						Comm::DUET.GetScaledPollInterval(),
						this);
	m_timers.receive = lv_timer_create(
		[](lv_timer_t* timer) { static_cast<Model*>(lv_timer_get_user_data(timer))->receiveNewUsbData(); }, 5, this);
#endif

	if (!initMutex())
	{
		fatal("Failed to initialise mutex");
	}
}

void Model::tick()
{
	lock();
	for (auto presenter : m_presenters)
	{
		presenter->tick();
	}
	unlock();
}

void Model::requestNewData()
{
	Comm::sendNext();
}

useconds_t Model::receiveNewUsbData()
{
	static constexpr size_t bufferSize = 32768;
	static Comm::JsonDecoder decoder;
	static BYTE buffer[bufferSize];
	static size_t bufferLen = 0;

	if (Comm::DUET.GetCommunicationType() != Comm::CommunicationType::usb)
	{
		return 500 * 1000;
	}

	if (!Comm::getCurrentUsbDevice().isConnected())
	{
		verbose("USB device disconnected");
		return 500 * 1000;
	}
	int len = Comm::getCurrentUsbDevice().receive(buffer + bufferLen, bufferSize - bufferLen);

	if (len > 0)
	{
		bufferLen += len;
		if (bufferLen >= bufferSize)
		{
			error("Buffer overflow");
			bufferLen = 0;
			return 5 * 1000;
		}
	}
	else if (len < 0)
	{
		error("Error receiving data");
		bufferLen = 0;
		return 5 * 1000;
	}
	if (buffer[bufferLen - 1] == '\n')
	{
		// Process the data
		decoder.CheckInput(buffer, bufferLen);
		bufferLen = 0;
	}
	return 5 * 1000;
}

void Model::runSubscribers(const char* key, Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	lock();
	auto subscribers = getSubscribers(key);
	if (subscribers.size() != 0)
	{
		dbg("found %d subscribers for '%s'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			subscriber.run(decoder, data, indices);
		}
	}
	unlock();
}

void Model::runArrayEndSubscribers(const char* key, Comm::JsonDecoder* decoder, const size_t indices[])
{
	lock();
	auto subscribers = getArrayEndSubscribers(key);
	if (subscribers.size() != 0)
	{
		dbg("found %d array end subscribers for '%s'", subscribers.size(), key);
		for (auto& subscriber : subscribers)
		{
			subscriber.run(decoder, indices);
		}
	}
	unlock();
}

/**
 * @brief Initializes a recursive mutex for the Model class.
 *
 * This function sets up a recursive mutex by initializing the mutex attributes,
 * setting the mutex type to recursive, and then initializing the mutex with these attributes.
 * If the initialization fails, an error code is logged and the function returns false.
 *
 * @return true if the mutex was successfully initialized, false otherwise.
 */
bool Model::initMutex()
{
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int ret = pthread_mutex_init(&m_mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	if (ret)
	{
		error("%d", ret);
		return false;
	}
	else
	{
		return true;
	}
}

void Model::lock()
{
	dbg("Attempting to lock model");
	lv_lock();
	pthread_mutex_lock(&m_mutex);
}

void Model::unlock()
{
	dbg("Unlocking model");
	lv_unlock();
	pthread_mutex_unlock(&m_mutex);
}

void Model::refresh()
{
	for (auto presenter : m_presenters)
	{
		presenter->refresh();
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

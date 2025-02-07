#pragma once

#include "ObjectModel/Alert.h"
#include "Subscribers/FanSubscribers.h"
#include "Subscribers/FileSubscribers.h"
#include "Subscribers/HeatSubscribers.h"
#include "Subscribers/JobSubscribers.h"
#include "Subscribers/MoveSubscribers.h"
#include "Subscribers/ResponseSubscribers.h"
#include "Subscribers/SensorSubscribers.h"
#include "Subscribers/SpindleSubscribers.h"
#include "Subscribers/StateSubscribers.h"
#include "Subscribers/Subscribers.h"
#include "Subscribers/ToolSubscribers.h"
#include "lvgl/lvgl.h"
#include <list>
#include <map>
#include <mutex>
#include <pthread.h>

namespace UI
{
	class BasePresenter;
}

class Model
{
  public:
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	static Model& get()
	{
		static Model instance;
		return instance;
	}

	/**
	 * @brief Add a `Presenter` to listen to events
	 * @param presenter
	 */
	void bind(UI::BasePresenter* presenter);

	/**
	 * @brief Remove a `Presenter`
	 * @param presenter
	 */
	void unbind(UI::BasePresenter* presenter);

	/* tasks */

	void tick();
	void requestNewData();
	useconds_t receiveNewUsbData();

	/* presenter callbacks */

	void refresh();

	/* Fan methods */

	void newFanData();

	/* File methods */

	void newFileData();

	/* Heater methods */

	void newHeaterData();

	/* Job methods */

	void newJobFileName();
	void newJobLastFileName();
	void newJobPrintTime();
	void newJobDuration();
	void newJobTimeLeft();
	void newJobWarmupDuration();
	void newJobBuild();
	void newJobCurrentObject();
	void newJobObjectData();

	/* Move methods */

	void newAxesData();
	void newExtruderData();
	void newKinematicsName();
	void newSpeedFactor();
	void newWorkplaceNumber();
	void newCurrentMoveRequestedSpeed();
	void newCurrentMoveTopSpeed();
	void newCurrentMoveExtrusionSpeed();
	void newCompensationFile();

	/* Response methods */

	void newResponse(const char* resp);

	/* Sensor methods */

	void newAnalogSensorData();
	void newEndstopData();

	/* Spindle methods */

	void newSpindleData();

	/* State methods */

	void newNetworkName();
	void newIpAddress();
	void newStatus();
	void newCurrentTool();
	void newMessageBoxData(const OM::Alert& alert);
	void newTime();

	/* Tool methods */

	void newToolData();

	/* Subscribers */

	void runSubscribers(const char* key, Comm::JsonDecoder* decoder, const char* data, const size_t indices[]);
	const std::vector<Subscriber>& getSubscribers(const char* key) { return SubscriberMap::getSubscribers(key); }
	const size_t getSubscriberCount(const char* key) { return SubscriberMap::getSubscriberCount(key); }

	void runArrayEndSubscribers(const char* key, Comm::JsonDecoder* decoder, const size_t indices[]);
	const std::vector<ArrayEndSubscriber>& getArrayEndSubscribers(const char* key)
	{
		return SubscriberMap::getArrayEndSubscribers(key);
	}
	const size_t getArrayEndSubscriberCount(const char* key) { return SubscriberMap::getArrayEndSubscriberCount(key); }

	void lock();
	void unlock();

  private:
	Model();
	bool initMutex();

	FanSubscribers m_fanSubscribers;
	FileSubscribers m_fileSubscribers;
	HeatSubscribers m_heatSubscribers;
	JobSubscribers m_jobSubscribers;
	MoveSubscribers m_moveSubscribers;
	ResponseSubscribers m_responseSubscribers;
	SensorSubscribers m_sensorSubscribers;
	SpindleSubscribers m_spindleSubscribers;
	StateSubscribers m_stateSubscribers;
	ToolSubscribers m_toolSubscribers;
	std::list<UI::BasePresenter*> m_presenters;

	pthread_mutex_t m_mutex;

	struct
	{
		lv_timer_t* tick;
		lv_timer_t* request;
		lv_timer_t* receive;
	} m_timers;
};

/**
 * @brief Creates a scopped lock for the model
 * @return Return a `ModelLock` object which calls `Model::get().lock()` on construction and `Model::get().unlock()` on
 * destruction
 */
struct ModelLock
{
	ModelLock()
		: m_model(Model::get())
	{
		m_model.lock();
	}
	~ModelLock() { m_model.unlock(); }

  private:
	Model& m_model;
};

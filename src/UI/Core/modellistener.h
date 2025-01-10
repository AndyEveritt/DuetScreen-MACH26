#pragma once

#include "model.h"

namespace UI
{
	class ModelListener
	{
	  public:
		ModelListener()
			: m_model(Model::get())
		{
		}

		virtual ~ModelListener() {}

		Model& getModel() { return m_model; }

		/* Fan methods */
		virtual void newFanData() {}

		/* File methods */
		virtual void newFileData() {}

		/* Heater methods */
		virtual void newHeaterData() {}

		/* Job methods */
		virtual void newJobFileName() {}
		virtual void newJobLastFileName() {}
		virtual void newJobPrintTime() {}
		virtual void newJobDuration() {}
		virtual void newJobTimeLeft() {}
		virtual void newJobWarmupDuration() {}
		virtual void newJobBuild() {}
		virtual void newJobCurrentObject() {}
		virtual void newJobObjectData() {}

		/* Move methods */
		virtual void newAxesData() {}
		virtual void newExtruderData() {}
		virtual void newKinematicsName() {}
		virtual void newSpeedFactor() {}
		virtual void newWorkplaceNumber() {}
		virtual void newCurrentMoveRequestedSpeed() {}
		virtual void newCurrentMoveTopSpeed() {}
		virtual void newCurrentMoveExtrusionSpeed() {}
		virtual void newCompensationFile() {}

		/* Response methods */
		virtual void newResponse() {}

		/* Sensor methods */
		virtual void newAnalogSensorData() {}
		virtual void newEndstopData() {}

		/* Spindle methods */
		virtual void newSpindleData() {}

		/* State methods */
		virtual void newNetworkName() {}
		virtual void newIpAddress() {}
		virtual void newStatus() {}
		virtual void newCurrentTool() {}
		virtual void newMessageBoxData() {}
		virtual void newTime() {}

		/* Tool methods */
		virtual void newToolData() {}

	  protected:
		Model& m_model;
	};
} // namespace UI

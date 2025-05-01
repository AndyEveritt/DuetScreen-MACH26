#pragma once

#include "Model.h"
#include "ObjectModel/Alert.h"

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

		Model& getModel() const { return m_model; }

		virtual void refresh() {}

		virtual void tick() {}

		virtual void newUpdateAvailable(const std::string& file) {}

		/* Fan methods */
		virtual void newFanData() {}

		/* File methods */
		virtual void newFileData() {}

		/* Heater methods */
		virtual void newHeaterData() {}

		/* Job methods */
		virtual void newJobFileName(const std::string& filename) {}
		virtual void newJobLastFileName(const std::string& filename) {}
		virtual void newJobPrintTime() {}
		virtual void newJobDuration() {}
		virtual void newJobTimeLeft() {}
		virtual void newJobWarmupDuration() {}
		virtual void newJobBuild() {}
		virtual void newJobCurrentObject() {}
		virtual void newJobObjectData() {}
		virtual void newThumbnailData(const std::string& filename) {}

		/* Move methods */
		virtual void newAxesData() {}
		virtual void newExtruderData() {}
		virtual void newKinematicsName() {}
		virtual void newSpeedFactor() {}
		virtual void newWorkplaceNumber() {}
		virtual void newPrintingAcceleration(const uint32_t& accel) {}
		virtual void newCurrentMoveRequestedSpeed() {}
		virtual void newCurrentMoveTopSpeed() {}
		virtual void newCurrentMoveExtrusionSpeed() {}
		virtual void newCompensationFile() {}

		/* Response methods */
		virtual void newResponse(const std::string& resp) {}
		virtual void newLogMessage(const Log::DebugLevel& level,
								   const Log::log_time_t& time,
								   const std::string& message)
		{
		}

		/* Sensor methods */
		virtual void newAnalogSensorData() {}
		virtual void newEndstopData() {}

		/* Spindle methods */
		virtual void newSpindleData() {}

		/* State methods */
		virtual void newNetworkName() {}
		virtual void newIpAddress() {}
		virtual void newStatus(const OM::PrinterStatus status) {}
		virtual void newCurrentTool() {}
		virtual void newMessageBoxData(const OM::Alert& alert) {}
		virtual void newTime() {}

		/* Tool methods */
		virtual void newToolData() {}

	  protected:
		Model& m_model;
	};
} // namespace UI

#include "Debug.h"

#include "JobSubscribers.h"
#include "ObjectModel/Job.h"
#include "UI/Core/Model.h"

bool JobSubscribers::currentFileName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::SetJobName(data);
	Model::get().newJobFileName();
	return true;
}

bool JobSubscribers::lastFileName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::SetLastJobName(data);
	Model::get().newJobLastFileName();
	return true;
}

bool JobSubscribers::printTime(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	OM::SetPrintTime(data);
	Model::get().newJobPrintTime();
	return true;
}

bool JobSubscribers::duration(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	OM::SetPrintDuration(data);
	Model::get().newJobDuration();
	return true;
}

bool JobSubscribers::slicerTimeLeft(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	OM::SetPrintRemaining(OM::RemainingTimeType::slicer, data);
	Model::get().newJobTimeLeft();
	return true;
}

bool JobSubscribers::warmUpDuration(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	OM::SetWarmUpDuration(data);
	Model::get().newJobWarmupDuration();
	return true;
}

bool JobSubscribers::nullBuild(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	dbg("Job: build is null");
	OM::RemoveJobObject(indices[0], true);
	Model::get().newJobBuild();
	return true;
}

bool JobSubscribers::currentObject(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	OM::SetCurrentJobObject(data);
	Model::get().newJobCurrentObject();
	return true;
}

bool JobSubscribers::nullObject(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::RemoveJobObject(indices[0], false);
	Model::get().newJobObjectData();
	return true;
}

bool JobSubscribers::objectCancelled(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	OM::JobObject* jobObject = OM::GetOrCreateJobObject(indices[0]);
	if (jobObject == nullptr)
	{
		warn("Job object %u not found", indices[0]);
	}
	jobObject->cancelled = data;
	return true;
}

bool JobSubscribers::objectName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::JobObject* jobObject = OM::GetOrCreateJobObject(indices[0]);
	if (jobObject == nullptr)
	{
		warn("Job object %u not found", indices[0]);
	}
	jobObject->name = data;
	return true;
}

bool JobSubscribers::objectX(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	OM::JobObject* jobObject = OM::GetOrCreateJobObject(indices[0]);
	if (jobObject == nullptr)
	{
		warn("Job object %u not found", indices[0]);
	}
	if (indices[1] < 0 || indices[1] >= 2)
	{
		warn("Job object %u x index %u out of range", indices[0], indices[1]);
		return false;
	}
	jobObject->bounds.x[indices[1]] = data;
	return true;
}

bool JobSubscribers::objectY(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	OM::JobObject* jobObject = OM::GetOrCreateJobObject(indices[0]);
	if (jobObject == nullptr)
	{
		warn("Job object %u not found", indices[0]);
	}
	if (indices[1] < 0 || indices[1] >= 2)
	{
		warn("Job object %u y index %u out of range", indices[0], indices[1]);
		return false;
	}
	jobObject->bounds.y[indices[1]] = data;
	return true;
}

bool JobSubscribers::objectArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	OM::RemoveJobObject(indices[0], true);
	Model::get().newJobObjectData();
	return true;
}

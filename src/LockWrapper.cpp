#include "LockWrapper.h"
#include "Debug.h"

DeadlockDetectingMutex mutexModel("MODEL_LOCK", true);
DeadlockDetectingMutex mutexUi("UI_LOCK", true);

// ScopedLock implementation
ScopedLock::ScopedLock(DeadlockDetectingMutex& lock)
	: mLock(lock)
{
	mLock.lock();
}

ScopedLock::~ScopedLock()
{
	mLock.unlock();
}

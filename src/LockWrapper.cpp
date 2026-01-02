#include "LockWrapper.h"
#include "Debug.h"

TracyLockable(DeadlockDetectingMutex<std::recursive_mutex>, mutexModel);
TracyLockable(DeadlockDetectingMutex<std::recursive_mutex>, mutexUi);

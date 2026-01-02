#pragma once

#include "DeadlockDetector.h"
#include "tracy/Tracy.hpp"

extern LockableBase(DeadlockDetectingMutex<std::recursive_mutex>) mutexModel;
extern LockableBase(DeadlockDetectingMutex<std::recursive_mutex>) mutexUi;

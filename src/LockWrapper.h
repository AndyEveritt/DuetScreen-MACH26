#pragma once

#include "DeadlockDetector.h"

extern DeadlockDetectingMutex mutexModel;
extern DeadlockDetectingMutex mutexUi;

// Helper for scope-based locking (to replace std::lock_guard if used)
class ScopedLock {
public:
    explicit ScopedLock(DeadlockDetectingMutex& lock);
    ~ScopedLock();
    
private:
    DeadlockDetectingMutex& mLock;
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
};

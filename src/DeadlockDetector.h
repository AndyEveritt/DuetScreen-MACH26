#pragma once

#include <mutex>
#include <thread>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>

class DeadlockDetector {
public:
    static DeadlockDetector& getInstance();
    void registerLock(const std::string& lockName, const void* lockPtr);
    void beforeLockAcquire(const void* lockPtr);
    void afterLockAcquire(const void* lockPtr);
    void beforeLockRelease(const void* lockPtr);
    void afterLockRelease(const void* lockPtr);

private:
    DeadlockDetector() = default;
    ~DeadlockDetector() = default;
    DeadlockDetector(const DeadlockDetector&) = delete;
    DeadlockDetector& operator=(const DeadlockDetector&) = delete;

    std::string getLockName(const void* lockPtr);
    void reportPotentialDeadlock(std::thread::id thread1, const void* lock1, 
                               std::thread::id thread2, const void* lock2);

    std::mutex mDetectorMutex;
    std::map<const void*, std::string> mRegisteredLocks;
    std::map<std::thread::id, std::set<const void*>> mThreadLocks;
    std::map<std::thread::id, const void*> mThreadWaiting;
};

// Wrapper for std::mutex to detect deadlocks
class DeadlockDetectingMutex {
public:
  explicit DeadlockDetectingMutex(const std::string& name, bool recursive = false);
  void lock();
  bool try_lock();
  void unlock();
  std::mutex& native_handle();

  // Updated method to check if mutex supports recursion
  bool is_recursive() const { return mRecursive; }

private:
    std::mutex mMutex;
    std::string mName;
	bool mRecursive;

	// Map to track thread-specific lock counts for recursive locking
	std::map<std::thread::id, int> mOwnershipCount;
	std::mutex mOwnershipMutex;
};

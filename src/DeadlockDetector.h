#pragma once

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>

class DeadlockDetector
{
  public:
	static DeadlockDetector& getInstance();
	void registerLock(const std::string& lockName, const void* lockPtr);
	void beforeLockAcquire(const void* lockPtr);
	void afterLockAcquire(const void* lockPtr);
	void beforeLockRelease(const void* lockPtr);
	void afterLockRelease(const void* lockPtr);
	void allowThreadToTakeMultipleLocks(std::thread::id threadId, bool allowed = true);

  private:
	DeadlockDetector() = default;
	~DeadlockDetector() = default;
	DeadlockDetector(const DeadlockDetector&) = delete;
	DeadlockDetector& operator=(const DeadlockDetector&) = delete;

	std::string getLockName(const void* lockPtr);
	void reportPotentialDeadlock(std::thread::id thread1,
								 const void* lock1,
								 std::thread::id thread2,
								 const void* lock2);

	std::mutex m_detectorMutex;
	std::map<const void*, std::string> m_registeredLocks;
	std::map<std::thread::id, std::set<const void*>> m_threadLocks;
	std::map<std::thread::id, const void*> m_threadWaiting;
	std::set<std::thread::id> m_threadsAllowedToTakeMultipleLocks;
};

// Wrapper for std::mutex to detect deadlocks
class DeadlockDetectingMutex
{
  public:
	explicit DeadlockDetectingMutex(const std::string& name, bool recursive = false);
	void lock();
	bool try_lock();
	void unlock();
	std::mutex& native_handle();

	// Updated method to check if mutex supports recursion
	bool is_recursive() const { return m_recursive; }

  private:
	std::mutex m_mutex;
	std::string m_name;
	bool m_recursive;

	// Map to track thread-specific lock counts for recursive locking
	std::map<std::thread::id, int> m_ownershipCount;
	std::mutex m_ownershipMutex;
};

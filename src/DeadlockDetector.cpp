#include "DeadlockDetector.h"
#include "Debug.h"
#include <chrono>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

DeadlockDetector& DeadlockDetector::getInstance()
{
	static DeadlockDetector instance;
	return instance;
}

void DeadlockDetector::registerLock(const std::string& lockName, const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(m_detectorMutex);
	m_registeredLocks[lockPtr] = lockName;
	verbose("Lock registered: %s at %p", lockName.c_str(), lockPtr);
}

void DeadlockDetector::beforeLockAcquire(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(m_detectorMutex);

	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);
	verbose("Thread %u attempting to acquire lock: %s", threadId, lockName.c_str());

	// Check if acquiring this lock might cause a deadlock
	if (m_threadLocks.find(threadId) != m_threadLocks.end())
	{
		verbose("Thread %u already holds %zu locks", threadId, m_threadLocks[threadId].size());

		auto threadLocks = m_threadLocks[threadId];

		if (threadLocks.find(lockPtr) == threadLocks.end() &&
			m_threadsAllowedToTakeMultipleLocks.find(threadId) == m_threadsAllowedToTakeMultipleLocks.end())
		{
			std::string ownedLocks;
			bool first = true;
			for (const auto& lock : threadLocks)
			{
				if (!first)
				{
					ownedLocks += ", ";
					first = false;
				}
				ownedLocks += getLockName(lock);
			}
			fatal("Thread %u is not allowed to take multiple locks, trying to take %s but already owns %s",
				  threadId,
				  lockName.c_str(),
				  ownedLocks.c_str());
			return;
		}
		// Check if any other thread holds the lock we want and is waiting for a lock we hold
		for (const auto& threadEntry : m_threadLocks)
		{
			if (threadEntry.first == threadId)
				continue;

			// Check if another thread holds the lock we want
			if (threadEntry.second.find(lockPtr) != threadEntry.second.end())
			{
				verbose(
					"Thread %u already holds the lock %s we're trying to acquire", threadEntry.first, lockName.c_str());

				// Check if that thread is waiting for any lock we hold
				for (const auto& waitEntry : m_threadWaiting)
				{
					if (waitEntry.first != threadEntry.first)
						continue;

					verbose("Thread %u is waiting for lock %s", waitEntry.first, getLockName(waitEntry.second).c_str());

					if (m_threadLocks[threadId].find(waitEntry.second) == m_threadLocks[threadId].end())
						continue;

					verbose(
						"Potential deadlock condition detected between threads %u and %u", threadId, threadEntry.first);

					reportPotentialDeadlock(threadId, lockPtr, threadEntry.first, waitEntry.second);
				}
			}
		}
	}

	// Record that this thread is waiting for this lock
	m_threadWaiting[threadId] = lockPtr;
	verbose("Thread %u now waiting for lock: %s", threadId, lockName.c_str());
}

void DeadlockDetector::afterLockAcquire(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(m_detectorMutex);
	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);

	m_threadLocks[threadId].insert(lockPtr);
	m_threadWaiting.erase(threadId);
	verbose("Thread %u acquired lock: %s (now holding %zu locks)",
			threadId,
			lockName.c_str(),
			m_threadLocks[threadId].size());
}

void DeadlockDetector::beforeLockRelease(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(m_detectorMutex);
	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);
	verbose("Thread %u about to release lock: %s", threadId, lockName.c_str());
}

void DeadlockDetector::afterLockRelease(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(m_detectorMutex);
	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);

	if (m_threadLocks.find(threadId) != m_threadLocks.end())
	{
		m_threadLocks[threadId].erase(lockPtr);
		size_t remainingLocks = m_threadLocks[threadId].size();
		verbose("Thread %u released lock: %s (still holding %zu locks)", threadId, lockName.c_str(), remainingLocks);

		if (remainingLocks == 0)
		{
			m_threadLocks.erase(threadId);
			verbose("Thread %u no longer holds any locks, removing from tracking", threadId);
		}
	}
}

std::string DeadlockDetector::getLockName(const void* lockPtr)
{
	auto it = m_registeredLocks.find(lockPtr);
	if (it != m_registeredLocks.end())
	{
		return it->second;
	}

	std::stringstream ss;
	ss << "Unknown Lock " << lockPtr;
	return ss.str();
}

void DeadlockDetector::reportPotentialDeadlock(std::thread::id thread1,
											   const void* lock1,
											   std::thread::id thread2,
											   const void* lock2)
{
	error("POTENTIAL DEADLOCK DETECTED!");
	error(
		"Thread %u holds %s and wants to acquire %s", thread1, getLockName(lock2).c_str(), getLockName(lock1).c_str());
	error(
		"Thread %u holds %s and wants to acquire %s", thread2, getLockName(lock1).c_str(), getLockName(lock2).c_str());
}

void DeadlockDetector::allowThreadToTakeMultipleLocks(std::thread::id threadId, bool allowed)
{
	std::lock_guard<std::mutex> guard(m_detectorMutex);
	if (allowed)
	{
		m_threadsAllowedToTakeMultipleLocks.insert(threadId);
	}
	else
	{
		m_threadsAllowedToTakeMultipleLocks.erase(threadId);
	}
	verbose("Thread %u %s allowed to take multiple locks", threadId, allowed ? "is" : "is not");
}

// DeadlockDetectingMutex implementation
DeadlockDetectingMutex::DeadlockDetectingMutex(const std::string& name, bool recursive)
	: m_name(name)
	, m_recursive(recursive)
{
	DeadlockDetector::getInstance().registerLock(name, &m_mutex);
	verbose("DeadlockDetectingMutex created: %s (recursive: %s)", name.c_str(), recursive ? "yes" : "no");
}

void DeadlockDetectingMutex::lock()
{
	auto threadId = std::this_thread::get_id();

	// Check if this is a recursive lock
	if (m_recursive)
	{
		std::lock_guard<std::mutex> guard(m_ownershipMutex);
		auto it = m_ownershipCount.find(threadId);
		if (it != m_ownershipCount.end() && it->second > 0)
		{
			// This thread already owns the lock - just increment the count
			it->second++;
			verbose("Thread %u recursively locked mutex %s (count: %d)", threadId, m_name.c_str(), it->second);
			return;
		}
	}

	// Normal lock acquisition path
	DeadlockDetector::getInstance().beforeLockAcquire(&m_mutex);

	// Try to acquire the lock with a timeout
	auto start = std::chrono::steady_clock::now();
	bool locked = false;

	while (!locked)
	{
		locked = m_mutex.try_lock();
		if (locked)
			break;

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

		if (elapsed > std::chrono::milliseconds(5000))
		{
			warn("Lock acquisition timeout for \"%s\" in thread %u", m_name.c_str(), std::this_thread::get_id());
			start = now; // Reset the timer to continue logging periodically
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Record ownership
	if (m_recursive)
	{
		std::lock_guard<std::mutex> guard(m_ownershipMutex);
		m_ownershipCount[threadId] = 1;
		verbose("Thread %u first-time lock of mutex %s", threadId, m_name.c_str());
	}

	DeadlockDetector::getInstance().afterLockAcquire(&m_mutex);
}

bool DeadlockDetectingMutex::try_lock()
{
	auto threadId = std::this_thread::get_id();

	// Check if this is a recursive lock
	if (m_recursive)
	{
		std::lock_guard<std::mutex> guard(m_ownershipMutex);
		auto it = m_ownershipCount.find(threadId);
		if (it != m_ownershipCount.end() && it->second > 0)
		{
			// This thread already owns the lock - just increment the count
			it->second++;
			verbose("Thread %u recursively try_locked mutex %s (count: %d)", threadId, m_name.c_str(), it->second);
			return true;
		}
	}

	bool result = m_mutex.try_lock();
	if (result)
	{
		if (m_recursive)
		{
			std::lock_guard<std::mutex> guard(m_ownershipMutex);
			m_ownershipCount[threadId] = 1;
			verbose("Thread %u first-time try_lock of mutex %s", threadId, m_name.c_str());
		}
		DeadlockDetector::getInstance().afterLockAcquire(&m_mutex);
	}
	return result;
}

void DeadlockDetectingMutex::unlock()
{
	if (m_recursive)
	{
		auto threadId = std::this_thread::get_id();
		bool actuallyUnlock = false;

		// Check if this is a recursive unlock
		{
			std::lock_guard<std::mutex> guard(m_ownershipMutex);
			auto it = m_ownershipCount.find(threadId);
			if (it != m_ownershipCount.end() && it->second > 0)
			{
				it->second--;
				verbose("Thread %u unlocked mutex %s (count: %d)", threadId, m_name.c_str(), it->second);

				if (it->second == 0)
				{
					// Last unlock, actually release the mutex
					actuallyUnlock = true;
					m_ownershipCount.erase(it);
					verbose("Thread %u final unlock of mutex %s", threadId, m_name.c_str());
				}
				else
				{
					// Still has locks, don't actually unlock
					return;
				}
			}
			else
			{
				error("Thread %u attempted to unlock mutex %s it doesn't own", threadId, m_name.c_str());
				return;
			}
		}

		if (!actuallyUnlock)
		{
			return;
		}
	}

	DeadlockDetector::getInstance().beforeLockRelease(&m_mutex);
	m_mutex.unlock();
	DeadlockDetector::getInstance().afterLockRelease(&m_mutex);
}

std::mutex& DeadlockDetectingMutex::native_handle()
{
	return m_mutex;
}

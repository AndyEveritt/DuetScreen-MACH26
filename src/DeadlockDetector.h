#pragma once

#include "Debug.h"
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>

using thread_id_t = size_t;

class DeadlockDetector
{
  public:
	static DeadlockDetector& getInstance();
	void registerLock(const std::string& lockName, const void* lockPtr);
	void beforeLockAcquire(const void* lockPtr);
	void afterLockAcquire(const void* lockPtr);
	void beforeLockRelease(const void* lockPtr);
	void afterLockRelease(const void* lockPtr);
	void allowThreadToTakeMultipleLocks(thread_id_t threadId, bool allowed = true);
	thread_id_t getOwningThreadId(const void* lockPtr);

  private:
	DeadlockDetector() = default;
	~DeadlockDetector() = default;
	DeadlockDetector(const DeadlockDetector&) = delete;
	DeadlockDetector& operator=(const DeadlockDetector&) = delete;

	std::string getLockName(const void* lockPtr);
	void reportPotentialDeadlock(thread_id_t thread1, const void* lock1, thread_id_t thread2, const void* lock2);

	std::mutex m_detectorMutex;
	std::map<const void*, std::string> m_registeredLocks;
	std::map<thread_id_t, std::set<const void*>> m_threadLocks;
	std::map<thread_id_t, const void*> m_threadWaiting;
	std::set<thread_id_t> m_threadsAllowedToTakeMultipleLocks;
};

// Wrapper for std::mutex to detect deadlocks
template <typename MutexType>
class DeadlockDetectingMutex
{
  public:
	DeadlockDetectingMutex()
	{
		DeadlockDetector::getInstance().registerLock(m_name, &m_mutex);
		LOG_VERBOSE("DeadlockDetectingMutex created: {:s} (recursive: {:s})", m_name, is_recursive() ? "yes" : "no");
	}

	void lock()
	{
		const auto threadId = Log::GetThreadId();

		// Check if this is a recursive lock
		if constexpr (is_recursive())
		{
			std::lock_guard<std::mutex> guard(m_ownershipMutex);
			auto it = m_ownershipCount.find(threadId);
			if (it != m_ownershipCount.end() && it->second > 0)
			{
				// This thread already owns the lock - just increment the count
				it->second++;
				LOG_VERBOSE("Thread {} recursively locked mutex {:s} (count: {:d})", threadId, m_name, it->second);
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
				thread_id_t holdingThread = DeadlockDetector::getInstance().getOwningThreadId(&m_mutex);
				LOG_WARN("Lock acquisition timeout for \"{:s}\" in thread {:d} (holding thread: {:d})",
						 m_name,
						 Log::GetThreadId(),
						 holdingThread);
				start = now; // Reset the timer to continue logging periodically
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		// Record ownership
		if constexpr (is_recursive())
		{
			std::lock_guard<std::mutex> guard(m_ownershipMutex);
			m_ownershipCount[threadId] = 1;
			LOG_VERBOSE("Thread {} first-time lock of mutex {:s}", threadId, m_name);
		}

		DeadlockDetector::getInstance().afterLockAcquire(&m_mutex);
	}
	bool try_lock()
	{
		auto threadId = Log::GetThreadId();

		// Check if this is a recursive lock
		if constexpr (is_recursive())
		{
			std::lock_guard<std::mutex> guard(m_ownershipMutex);
			auto it = m_ownershipCount.find(threadId);
			if (it != m_ownershipCount.end() && it->second > 0)
			{
				// This thread already owns the lock - just increment the count
				it->second++;
				LOG_VERBOSE("Thread {} recursively try_locked mutex {:s} (count: {:d})", threadId, m_name, it->second);
				return true;
			}
		}

		bool result = m_mutex.try_lock();
		if (result)
		{
			if constexpr (is_recursive())
			{
				std::lock_guard<std::mutex> guard(m_ownershipMutex);
				m_ownershipCount[threadId] = 1;
				LOG_VERBOSE("Thread {} first-time try_lock of mutex {:s}", threadId, m_name);
			}
			DeadlockDetector::getInstance().afterLockAcquire(&m_mutex);
		}
		return result;
	}
	void unlock()
	{

		if constexpr (is_recursive())
		{
			auto threadId = Log::GetThreadId();
			bool actuallyUnlock = false;

			// Check if this is a recursive unlock
			{
				std::lock_guard<std::mutex> guard(m_ownershipMutex);
				auto it = m_ownershipCount.find(threadId);
				if (it != m_ownershipCount.end() && it->second > 0)
				{
					it->second--;
					LOG_VERBOSE("Thread {} unlocked mutex {:s} (count: {:d})", threadId, m_name, it->second);

					if (it->second == 0)
					{
						// Last unlock, actually release the mutex
						actuallyUnlock = true;
						m_ownershipCount.erase(it);
						LOG_VERBOSE("Thread {} final unlock of mutex {:s}", threadId, m_name);
					}
					else
					{
						// Still has locks, don't actually unlock
						return;
					}
				}
				else
				{
					LOG_ERROR("Thread {} attempted to unlock mutex {:s} it doesn't own", threadId, m_name);
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

	MutexType& native_handle() { return m_mutex; }
	const MutexType& native_handle() const { return m_mutex; }

	static consteval bool is_recursive() { return std::is_base_of_v<std::recursive_mutex, MutexType>; }

  private:
	MutexType m_mutex;
	std::string m_name;

	// Map to track thread-specific lock counts for recursive locking
	std::map<thread_id_t, int> m_ownershipCount;
	std::mutex m_ownershipMutex;
};

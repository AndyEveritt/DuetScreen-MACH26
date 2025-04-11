#include "DeadlockDetector.h"
#include "Debug.h"

// DeadlockDetector implementation
DeadlockDetector& DeadlockDetector::getInstance()
{
	static DeadlockDetector instance;
	return instance;
}

void DeadlockDetector::registerLock(const std::string& lockName, const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(mDetectorMutex);
	mRegisteredLocks[lockPtr] = lockName;
	verbose("Lock registered: %s at %p", lockName.c_str(), lockPtr);
}

void DeadlockDetector::beforeLockAcquire(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(mDetectorMutex);

	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);
	verbose("Thread %u attempting to acquire lock: %s", threadId, lockName.c_str());

	// Check if acquiring this lock might cause a deadlock
	if (mThreadLocks.find(threadId) != mThreadLocks.end())
	{
		verbose("Thread %u already holds %zu locks", threadId, mThreadLocks[threadId].size());
		// Check if any other thread holds the lock we want and is waiting for a lock we hold
		for (const auto& threadEntry : mThreadLocks)
		{
			if (threadEntry.first == threadId)
				continue;

			// Check if another thread holds the lock we want
			if (threadEntry.second.find(lockPtr) != threadEntry.second.end())
			{
				verbose(
					"Thread %u already holds the lock %s we're trying to acquire", threadEntry.first, lockName.c_str());

				// Check if that thread is waiting for any lock we hold
				for (const auto& waitEntry : mThreadWaiting)
				{
					if (waitEntry.first != threadEntry.first)
						continue;

					verbose("Thread %u is waiting for lock %s", waitEntry.first, getLockName(waitEntry.second).c_str());

					if (mThreadLocks[threadId].find(waitEntry.second) == mThreadLocks[threadId].end())
						continue;

					verbose(
						"Potential deadlock condition detected between threads %u and %u", threadId, threadEntry.first);

					reportPotentialDeadlock(threadId, lockPtr, threadEntry.first, waitEntry.second);
				}
			}
		}
	}

	// Record that this thread is waiting for this lock
	mThreadWaiting[threadId] = lockPtr;
	verbose("Thread %u now waiting for lock: %s", threadId, lockName.c_str());
}

void DeadlockDetector::afterLockAcquire(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(mDetectorMutex);
	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);

	mThreadLocks[threadId].insert(lockPtr);
	mThreadWaiting.erase(threadId);
	verbose("Thread %u acquired lock: %s (now holding %zu locks)",
			threadId,
			lockName.c_str(),
			mThreadLocks[threadId].size());
}

void DeadlockDetector::beforeLockRelease(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(mDetectorMutex);
	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);
	verbose("Thread %u about to release lock: %s", threadId, lockName.c_str());
}

void DeadlockDetector::afterLockRelease(const void* lockPtr)
{
	std::lock_guard<std::mutex> guard(mDetectorMutex);
	auto threadId = std::this_thread::get_id();
	std::string lockName = getLockName(lockPtr);

	if (mThreadLocks.find(threadId) != mThreadLocks.end())
	{
		mThreadLocks[threadId].erase(lockPtr);
		size_t remainingLocks = mThreadLocks[threadId].size();
		verbose("Thread %u released lock: %s (still holding %zu locks)", threadId, lockName.c_str(), remainingLocks);

		if (remainingLocks == 0)
		{
			mThreadLocks.erase(threadId);
			verbose("Thread %u no longer holds any locks, removing from tracking", threadId);
		}
	}
}

std::string DeadlockDetector::getLockName(const void* lockPtr)
{
	auto it = mRegisteredLocks.find(lockPtr);
	if (it != mRegisteredLocks.end())
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

// DeadlockDetectingMutex implementation
DeadlockDetectingMutex::DeadlockDetectingMutex(const std::string& name, bool recursive)
	: mName(name)
	, mRecursive(recursive)
{
	DeadlockDetector::getInstance().registerLock(name, &mMutex);
	verbose("DeadlockDetectingMutex created: %s (recursive: %s)", name.c_str(), recursive ? "yes" : "no");
}

void DeadlockDetectingMutex::lock()
{
	auto threadId = std::this_thread::get_id();

	// Check if this is a recursive lock
	if (mRecursive)
	{
		std::lock_guard<std::mutex> guard(mOwnershipMutex);
		auto it = mOwnershipCount.find(threadId);
		if (it != mOwnershipCount.end() && it->second > 0)
		{
			// This thread already owns the lock - just increment the count
			it->second++;
			verbose("Thread %u recursively locked mutex %s (count: %d)", threadId, mName.c_str(), it->second);
			return;
		}
	}

	// Normal lock acquisition path
	DeadlockDetector::getInstance().beforeLockAcquire(&mMutex);

	// Try to acquire the lock with a timeout
	auto start = std::chrono::steady_clock::now();
	bool locked = false;

	while (!locked)
	{
		locked = mMutex.try_lock();
		if (locked)
			break;

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

		if (elapsed > std::chrono::milliseconds(5000))
		{
			error("Lock acquisition timeout for \"%s\" in thread %u", mName.c_str(), std::this_thread::get_id());
			start = now; // Reset the timer to continue logging periodically
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Record ownership
	if (mRecursive)
	{
		std::lock_guard<std::mutex> guard(mOwnershipMutex);
		mOwnershipCount[threadId] = 1;
		verbose("Thread %u first-time lock of mutex %s", threadId, mName.c_str());
	}

	DeadlockDetector::getInstance().afterLockAcquire(&mMutex);
}

bool DeadlockDetectingMutex::try_lock()
{
	auto threadId = std::this_thread::get_id();

	// Check if this is a recursive lock
	if (mRecursive)
	{
		std::lock_guard<std::mutex> guard(mOwnershipMutex);
		auto it = mOwnershipCount.find(threadId);
		if (it != mOwnershipCount.end() && it->second > 0)
		{
			// This thread already owns the lock - just increment the count
			it->second++;
			verbose("Thread %u recursively try_locked mutex %s (count: %d)", threadId, mName.c_str(), it->second);
			return true;
		}
	}

	bool result = mMutex.try_lock();
	if (result)
	{
		if (mRecursive)
		{
			std::lock_guard<std::mutex> guard(mOwnershipMutex);
			mOwnershipCount[threadId] = 1;
			verbose("Thread %u first-time try_lock of mutex %s", threadId, mName.c_str());
		}
		DeadlockDetector::getInstance().afterLockAcquire(&mMutex);
	}
	return result;
}

void DeadlockDetectingMutex::unlock()
{
	if (mRecursive)
	{
		auto threadId = std::this_thread::get_id();
		bool actuallyUnlock = false;

		// Check if this is a recursive unlock
		{
			std::lock_guard<std::mutex> guard(mOwnershipMutex);
			auto it = mOwnershipCount.find(threadId);
			if (it != mOwnershipCount.end() && it->second > 0)
			{
				it->second--;
				verbose("Thread %u unlocked mutex %s (count: %d)", threadId, mName.c_str(), it->second);

				if (it->second == 0)
				{
					// Last unlock, actually release the mutex
					actuallyUnlock = true;
					mOwnershipCount.erase(it);
					verbose("Thread %u final unlock of mutex %s", threadId, mName.c_str());
				}
				else
				{
					// Still has locks, don't actually unlock
					return;
				}
			}
			else
			{
				error("Thread %u attempted to unlock mutex %s it doesn't own", threadId, mName.c_str());
				return;
			}
		}

		if (!actuallyUnlock)
		{
			return;
		}
	}

	DeadlockDetector::getInstance().beforeLockRelease(&mMutex);
	mMutex.unlock();
	DeadlockDetector::getInstance().afterLockRelease(&mMutex);
}

std::mutex& DeadlockDetectingMutex::native_handle()
{
	return mMutex;
}

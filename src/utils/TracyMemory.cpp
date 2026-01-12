#include "TracyMemory.h"

#if defined(TRACY_ENABLE) && TRACY_ENABLE

#  include <atomic>
#  include <chrono>
#  include <cstddef>
#  include <cstdlib>
#  include <new>
#  include <thread>
#  include <unistd.h>
#  if defined(__GLIBC__)
#	include <malloc.h>
#  endif

#  include "tracy/Tracy.hpp"

namespace
{
	std::atomic<bool> g_running{false};
	std::thread g_sampler;

	static inline int64_t getRssBytes() noexcept
	{
		// Linux: /proc/self/statm -> resident pages * page size
		FILE* f = std::fopen("/proc/self/statm", "r");
		if (!f)
			return 0;
		long size = 0, resident = 0;
		int rc = std::fscanf(f, "%ld %ld", &size, &resident);
		std::fclose(f);
		if (rc != 2)
			return 0;
		const long page = sysconf(_SC_PAGESIZE);
		if (resident < 0 || page <= 0)
			return 0;
		return resident * page;
	}

	static inline int64_t getHeapUsedBytes() noexcept
	{
#  if defined(__GLIBC__)
#	if (__GLIBC__ * 100 + __GLIBC_MINOR__) >= 233
		// mallinfo2 is available since glibc 2.33
		auto mi = mallinfo2();
		return static_cast<int64_t>(mi.uordblks);
#	else
		auto mi = mallinfo();
		return static_cast<int64_t>(mi.uordblks);
#	endif
#  else
		return 0;
#  endif
	}

} // namespace

namespace TracyMemory
{

	void Start()
	{
		bool expected = false;
		if (!g_running.compare_exchange_strong(expected, true))
			return;

		// Configure plots as memory units (no step, no fill, default color)
		TracyPlotConfig("rss_bytes", tracy::PlotFormatType::Memory, false, false, 0);
		TracyPlotConfig("heap_used", tracy::PlotFormatType::Memory, false, false, 0);

		g_sampler = std::thread(
			[]
			{
				using namespace std::chrono_literals;
				while (g_running.load(std::memory_order_relaxed))
				{
					const int64_t rss = getRssBytes();
					if (rss > 0)
					{
						TracyPlot("rss_bytes", rss);
					}
					const int64_t heap = getHeapUsedBytes();
					if (heap > 0)
					{
						TracyPlot("heap_used", heap);
					}
					std::this_thread::sleep_for(200ms);
				}
			});
		g_sampler.detach();
	}

	void Stop()
	{
		if (!g_running.exchange(false))
			return;
	}

} // namespace TracyMemory

// Global new/delete overrides to attribute allocations to Tracy in Simulation builds.
// Kept simple and noexcept to avoid surprises; only compiled when TRACY is enabled.

void* operator new(std::size_t sz)
{
	if (void* p = std::malloc(sz))
	{
		TracyAlloc(p, sz);
		return p;
	}
	throw std::bad_alloc();
}

void* operator new[](std::size_t sz)
{
	if (void* p = std::malloc(sz))
	{
		TracyAlloc(p, sz);
		return p;
	}
	throw std::bad_alloc();
}

void* operator new(std::size_t sz, const std::nothrow_t&) noexcept
{
	void* p = std::malloc(sz);
	if (p)
		TracyAlloc(p, sz);
	return p;
}

void* operator new[](std::size_t sz, const std::nothrow_t&) noexcept
{
	void* p = std::malloc(sz);
	if (p)
		TracyAlloc(p, sz);
	return p;
}

void operator delete(void* p) noexcept
{
	if (p)
	{
		TracyFree(p);
		std::free(p);
	}
}

void operator delete[](void* p) noexcept
{
	if (p)
	{
		TracyFree(p);
		std::free(p);
	}
}

// Sized delete overloads (C++14+). Ensure correct matching when compiler uses sized delete.
void operator delete(void* p, std::size_t) noexcept
{
	if (p)
	{
		TracyFree(p);
		std::free(p);
	}
}

void operator delete[](void* p, std::size_t) noexcept
{
	if (p)
	{
		TracyFree(p);
		std::free(p);
	}
}

#endif // TRACY_ENABLE

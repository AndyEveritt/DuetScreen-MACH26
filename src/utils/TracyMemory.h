#pragma once

#if defined(TRACY_ENABLE) && TRACY_ENABLE
namespace TracyMemory {
void Start();
void Stop();
}
#endif

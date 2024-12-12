#pragma once

#include <functional>

namespace UI
{
	typedef std::function<void(const char* data, const size_t arrayIndices[])> subscriberCb_t;
} // namespace UI

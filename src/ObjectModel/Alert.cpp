#include "Debug.h"

#include "Alert.h"
#include "utils/utils.h"

namespace OM
{
	Alert g_currentAlert;
	uint32_t g_lastAlertSeq = 0;

	void Alert::Reset()
	{
		LOG_VERBOSE("Resetting alert");
		mode = Mode::None;
		flags.Clear();
		for (size_t i = 0; i < ARRAY_SIZE(choices); i++)
		{
			choices[i].Clear();
		}
		limits.numberInt.min = std::numeric_limits<int32_t>::min();
		limits.numberInt.max = std::numeric_limits<int32_t>::max();
		limits.numberFloat.min = std::numeric_limits<float>::lowest();
		limits.numberFloat.max = std::numeric_limits<float>::max();
		limits.text.min = 0;
		limits.text.max = std::numeric_limits<int32_t>::max();
	}
} // namespace OM

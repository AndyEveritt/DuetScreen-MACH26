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
		limits.numberInt.min = INT32_MIN;
		limits.numberInt.max = INT32_MAX;
		limits.numberFloat.min = -FLT_MAX;
		limits.numberFloat.max = FLT_MAX;
		limits.text.min = 0;
		limits.text.max = INT32_MAX;
	}
} // namespace OM

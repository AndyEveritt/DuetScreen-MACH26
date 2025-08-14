/*
 * UiTestSuite.cpp
 *
 *  Created on: 2025-08-14
 *      Author: Andy Everitt
 */

#include "UiTestSuite.h"
#include "Configuration.h"
#include "DeadlockDetector.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "test_utils/utils.h"

void UiTestSuite::SetUpTestSuite()
{
	lv_init();

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
	/* Disable profiler, to reduce redundant profiler log printing  */
	lv_profiler_builtin_set_enable(false);
#endif

	lv_display_t* display = lv_test_display_create(HOR_RES, VER_RES);
	lv_test_indev_create_all();

#if LV_USE_GESTURE_RECOGNITION
	lv_test_indev_gesture_create();
#endif

#if LV_USE_SYSMON
#  if LV_USE_MEM_MONITOR
	lv_sysmon_hide_memory(NULL);
#  endif
#  if LV_USE_PERF_MONITOR
	lv_sysmon_hide_performance(NULL);
#  endif
#endif

	lv_i18n_init(lv_i18n_language_pack);
	lv_i18n_set_locale(DEFAULT_LANGUAGE_CODE);

	DeadlockDetector::getInstance().allowThreadToTakeMultipleLocks(Log::GetThreadId(), true);

	UI::Themes::init(display);
}

void UiTestSuite::TearDownTestSuite()
{
	// Cleanup
	lv_mem_deinit();
}

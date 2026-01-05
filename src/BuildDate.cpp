/*
 * BuildDate.cpp
 *
 *  Created on: 2025-07-25
 *      Author: Andy Everitt
 */

#include "BuildDate.h"
#include "Debug.h"
#include <Duet3D/General/IsoDate.h>

#ifndef USE_FIXED_TEST_BUILD_TIME
#  define USE_FIXED_TEST_BUILD_TIME 0
#endif

#if USE_FIXED_TEST_BUILD_TIME
const char* const BuildDateText = "YYYY-MM-DD";
const char* const BuildTimeSuffix = "HH:MM:SS";
#else
const char* const BuildDateText = IsoDate;
const char* const BuildTimeSuffix = __TIME__;
#endif
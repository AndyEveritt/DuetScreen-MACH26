/*
 * BuildDate.cpp
 *
 *  Created on: 2025-07-25
 *      Author: Andy Everitt
 */

#include "BuildDate.h"
#include "Debug.h"
#include <Duet3D/General/IsoDate.h>

const char* const BuildDateText = IsoDate;
const char* const BuildTimeSuffix = __TIME__;

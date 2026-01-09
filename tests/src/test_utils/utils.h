/*
 * utils.h
 *
 *  Created on: 2025-08-14
 *      Author: Andy Everitt
 */

#pragma once

#include "Debug.h"
#include "UI/Core/Model.h"
#include "lvgl/lvgl.h"
#include <gtest/gtest.h>

#define HOR_RES 1024
#define VER_RES 600

#if LV_COLOR_DEPTH != 32
#  define EXPECT_EQUAL_SCREENSHOT(path) TEST_IGNORE_MESSAGE("Requires LV_COLOR_DEPTH 32");
#else
#  define EXPECT_EQUAL_SCREENSHOT(path)                                                                                \
	  if (LV_HOR_RES != HOR_RES || LV_VER_RES != VER_RES)                                                              \
	  {                                                                                                                \
		  LOG_ERROR("Requires {:d}x{:d} resolution", HOR_RES, VER_RES);                                                \
	  }                                                                                                                \
	  else                                                                                                             \
	  {                                                                                                                \
		  Model::get().startEventLoop();                                                                               \
		  Model::get().stopEventLoop();                                                                                \
		  UI_LOCK();                                                                                                   \
		  EXPECT_TRUE(lv_test_screenshot_compare(path));                                                               \
	  }
#endif
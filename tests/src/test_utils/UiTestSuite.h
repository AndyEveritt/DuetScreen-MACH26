/*
 * UiTestSuite.h
 *
 *  Created on: 2025-08-14
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Screen/Screen.h"
#include "test_utils/TestSuite.h"
#include "test_utils/utils.h"
#include <gtest/gtest.h>

class UiTestSuite : public TestSuite
{
  protected:
	UiTestSuite();

	virtual ~UiTestSuite();

	static void SetUpTestSuite();

	static void TearDownTestSuite();

	UI::Screen screen{"test_screen"};
};

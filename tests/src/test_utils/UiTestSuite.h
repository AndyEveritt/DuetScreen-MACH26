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

class UiTestSuiteInner : public TestSuite
{
  protected:
	UiTestSuiteInner();
	virtual ~UiTestSuiteInner();
};

class UiTestSuite : public UiTestSuiteInner
{
  protected:
	UiTestSuite();
	virtual ~UiTestSuite() = default;

	UI::Screen screen{"test_screen"};
};

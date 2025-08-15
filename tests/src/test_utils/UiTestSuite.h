/*
 * UiTestSuite.h
 *
 *  Created on: 2025-08-14
 *      Author: Andy Everitt
 */

#pragma once

#include "test_utils/utils.h"
#include <gtest/gtest.h>

class UiTestSuite : public ::testing::Test
{
  protected:
	UiTestSuite() = default;
	
	virtual ~UiTestSuite() = default;

	static void SetUpTestSuite();

	static void TearDownTestSuite();
};

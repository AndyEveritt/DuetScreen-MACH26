/*
 * test_communication.cpp
 *
 *  Created on: 2025-10-07
 *      Author: Andy Everitt
 */

#include "Comm/Communication.h"
#include "Debug.h"
#include "test_utils/TestSuite.h"
#include <gtest/gtest.h>

class TestCommunication : public TestSuite
{
  public:
	TestCommunication() {}
};

TEST_F(TestCommunication, SendNext)
{
	auto seq = Comm::GetNextSeq(nullptr);

	EXPECT_EQ(strcasecmp(seq->key, "network"), 0);
}
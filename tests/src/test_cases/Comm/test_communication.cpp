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
	auto seq = Comm::GetNextSeq(Comm::g_currentReqSeq);
	EXPECT_STREQ(seq->key, "network");

	/* Test that the sequence doesn't advance if current one not sent */
	seq = Comm::GetNextSeq(Comm::g_currentReqSeq);
	EXPECT_STREQ(seq->key, "network");
	EXPECT_EQ(seq->state, Comm::SeqState::SeqStateInit);

	EXPECT_TRUE(Comm::sendNext());
	ASSERT_NE(Comm::g_currentReqSeq, nullptr);
	EXPECT_EQ(Comm::g_currentReqSeq->state, Comm::SeqState::SeqStateRequested);
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "network");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "boards");
	auto boards_seq = Comm::g_currentReqSeq;

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "move");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "heat");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "tools");
	auto tools_seq = Comm::g_currentReqSeq;

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "spindles");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "directories");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "fans");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "job");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "sensors");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "state");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "volumes");

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_STREQ(Comm::g_currentReqSeq->key, "");

	/* Test that sendNext returns false when there are no more sequences */
	EXPECT_FALSE(Comm::sendNext());
	EXPECT_EQ(Comm::g_currentReqSeq, nullptr);

	EXPECT_FALSE(Comm::sendNext());

	/* boards state should be set to init and skip a go so next seq should be tools */
	boards_seq->state = Comm::SeqState::SeqStateError;
	tools_seq->state = Comm::SeqState::SeqStateUpdate;
	seq = Comm::GetNextSeq(Comm::g_currentReqSeq);
	EXPECT_STREQ(seq->key, "tools");
	EXPECT_EQ(boards_seq->state, Comm::SeqState::SeqStateInit);

	seq = Comm::GetNextSeq(Comm::g_currentReqSeq);
	EXPECT_STREQ(seq->key, "boards");
	EXPECT_TRUE(Comm::sendNext()); // this will send `boards` because the error state was cleared when we manually
								   // called GetNextSeq()
	seq = Comm::GetNextSeq(Comm::g_currentReqSeq);
	EXPECT_STREQ(seq->key, "tools");
}

TEST_F(TestCommunication, GetInteger)
{
	int32_t val = 0;

	EXPECT_FALSE(Comm::GetInteger(nullptr, val));
	EXPECT_EQ(val, 0);

	EXPECT_FALSE(Comm::GetInteger("", val));
	EXPECT_EQ(val, 0);

	/* Test that values beyond the int32_t range are not accepted */
	EXPECT_TRUE(Comm::GetInteger("2147483647", val));
	EXPECT_EQ(val, 2147483647);
	EXPECT_FALSE(Comm::GetInteger("2147483648", val));
	EXPECT_FALSE(Comm::GetInteger("123456789012", val));
	EXPECT_TRUE(Comm::GetInteger("-2147483648", val));
	EXPECT_EQ(val, -2147483648);
	EXPECT_FALSE(Comm::GetInteger("-2147483649", val));

	EXPECT_TRUE(Comm::GetInteger("123", val));
	EXPECT_EQ(val, 123);

	EXPECT_TRUE(Comm::GetInteger("-123", val));
	EXPECT_EQ(val, -123);

	/* Test that leading whitespace is ignored */
	EXPECT_TRUE(Comm::GetInteger("   123", val));
	EXPECT_EQ(val, 123);

	EXPECT_TRUE(Comm::GetInteger("   -123", val));
	EXPECT_EQ(val, -123);

	/* Test that trailing whitespace is not accepted */
	EXPECT_FALSE(Comm::GetInteger("123   ", val));

	EXPECT_FALSE(Comm::GetInteger("-123   ", val));

	/* Test that non-numeric strings are not accepted */
	EXPECT_FALSE(Comm::GetInteger("123abc", val));

	EXPECT_FALSE(Comm::GetInteger("-123abc", val));

	/* Test that floating point numbers are rounded */
	EXPECT_TRUE(Comm::GetInteger("   123.45", val));
	EXPECT_EQ(val, 123);

	EXPECT_TRUE(Comm::GetInteger("   123.54", val));
	EXPECT_EQ(val, 124);

	EXPECT_TRUE(Comm::GetInteger("   -123.45", val));
	EXPECT_EQ(val, -123);

	EXPECT_TRUE(Comm::GetInteger("   -123.54", val));
	EXPECT_EQ(val, -124);

	/* Test scientific format */
	EXPECT_TRUE(Comm::GetInteger("1.2345e2", val));
	EXPECT_EQ(val, 123);
}

TEST_F(TestCommunication, GetUnsignedInteger)
{
	uint32_t val = 0;

	EXPECT_FALSE(Comm::GetUnsignedInteger(nullptr, val));
	EXPECT_EQ(val, 0);

	EXPECT_FALSE(Comm::GetUnsignedInteger("", val));
	EXPECT_EQ(val, 0);

	/* Test that values beyond the uint32_t range are not accepted */
	EXPECT_TRUE(Comm::GetUnsignedInteger("4294967295", val));
	EXPECT_EQ(val, 4294967295U);
	EXPECT_FALSE(Comm::GetUnsignedInteger("4294967296", val));
	EXPECT_FALSE(Comm::GetUnsignedInteger("12345678901", val));
	EXPECT_FALSE(Comm::GetUnsignedInteger("-1", val));

	EXPECT_TRUE(Comm::GetUnsignedInteger("123", val));
	EXPECT_EQ(val, 123);

	/* Test that leading whitespace is ignored */
	EXPECT_TRUE(Comm::GetUnsignedInteger("   123", val));
	EXPECT_EQ(val, 123);

	/* Test that trailing whitespace is not accepted */
	EXPECT_FALSE(Comm::GetUnsignedInteger("123   ", val));

	/* Test that floating point numbers are rounded */
	EXPECT_TRUE(Comm::GetUnsignedInteger("   123.45", val));
	EXPECT_EQ(val, 123);

	EXPECT_TRUE(Comm::GetUnsignedInteger("   123.54", val));
	EXPECT_EQ(val, 124);

	/* Test scientific format */
	EXPECT_TRUE(Comm::GetUnsignedInteger("1.2345e2", val));
	EXPECT_EQ(val, 123);
}

TEST_F(TestCommunication, GetBool)
{
	bool val = false;

	EXPECT_FALSE(Comm::GetBool(nullptr, val));

	EXPECT_FALSE(Comm::GetBool("", val));

	EXPECT_TRUE(Comm::GetBool("true", val));
	EXPECT_EQ(val, true);

	EXPECT_TRUE(Comm::GetBool("false", val));
	EXPECT_EQ(val, false);

	EXPECT_TRUE(Comm::GetBool("True", val));
	EXPECT_EQ(val, true);

	EXPECT_TRUE(Comm::GetBool("FALSE", val));
	EXPECT_EQ(val, false);

	EXPECT_TRUE(Comm::GetBool("yes", val));
	EXPECT_EQ(val, false);
}

TEST_F(TestCommunication, GetFloat)
{
	float val = 0.0f;

	EXPECT_FALSE(Comm::GetFloat(nullptr, val));
	EXPECT_EQ(val, 0.0f);

	EXPECT_FALSE(Comm::GetFloat("", val));
	EXPECT_EQ(val, 0.0f);

	EXPECT_TRUE(Comm::GetFloat("123.45", val));
	EXPECT_FLOAT_EQ(val, 123.45f);

	EXPECT_TRUE(Comm::GetFloat("-123.45", val));
	EXPECT_FLOAT_EQ(val, -123.45f);

	/* Test that leading whitespace is ignored */
	EXPECT_TRUE(Comm::GetFloat("   123.45", val));
	EXPECT_FLOAT_EQ(val, 123.45f);

	EXPECT_TRUE(Comm::GetFloat("   -123.45", val));
	EXPECT_FLOAT_EQ(val, -123.45f);

	/* Test that trailing whitespace is not accepted */
	EXPECT_FALSE(Comm::GetFloat("123.45   ", val));

	EXPECT_FALSE(Comm::GetFloat("-123.45   ", val));

	/* Test that non-numeric strings are not accepted */
	EXPECT_FALSE(Comm::GetFloat("123.45abc", val));

	EXPECT_FALSE(Comm::GetFloat("-123.45abc", val));

	/* Test scientific format */
	EXPECT_TRUE(Comm::GetFloat("1.2345e2", val));
	EXPECT_FLOAT_EQ(val, 123.45f);
}
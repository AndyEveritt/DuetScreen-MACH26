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
	EXPECT_STREQ(seq->key, "network");

	/* Test that the sequence doesn't advance if current one not sent */
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "network");
	EXPECT_EQ(seq->state, Comm::SeqState::SeqStateInit);

	EXPECT_TRUE(Comm::sendNext());
	EXPECT_EQ(seq->state, Comm::SeqState::SeqStateRequested);
	seq = Comm::GetNextSeq(seq);
	auto boards_seq = seq;
	EXPECT_STREQ(seq->key, "boards");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "move");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "heat");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	auto tools_seq = seq;
	EXPECT_STREQ(seq->key, "tools");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "spindles");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "directories");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "fans");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "job");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "sensors");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "state");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "volumes");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "");

	EXPECT_TRUE(Comm::sendNext());
	seq = Comm::GetNextSeq(seq);
	EXPECT_EQ(seq, nullptr);

	/* Test that sendNext returns false when there are no more sequences */
	EXPECT_FALSE(Comm::sendNext());

	/* boards state should be set to init and skip a go so next seq should be tools */
	boards_seq->state = Comm::SeqState::SeqStateError;
	tools_seq->state = Comm::SeqState::SeqStateUpdate;
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "tools");
	EXPECT_EQ(boards_seq->state, Comm::SeqState::SeqStateInit);

	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "boards");
	EXPECT_TRUE(Comm::sendNext()); // this will send `boards` because the error state was cleared when we manually
								   // called GetNextSeq()
	seq = Comm::GetNextSeq(seq);
	EXPECT_STREQ(seq->key, "tools");
}
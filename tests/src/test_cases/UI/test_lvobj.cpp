/*
 * test_lvobj.cpp
 *
 *  Created on: 2025-10-09
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class TestLvobj : public UiTestSuite
{
  public:
	TestLvobj() {}
};

TEST_F(TestLvobj, FindChildByName)
{
	LvContainer parent("parent", screen);
	LvContainer child1("child1", parent);
	LvContainer child2("child2", parent);
	LvContainer grandchild("grandchild", child1);

	EXPECT_EQ(parent.getChildByName("child1"), &child1);
	EXPECT_EQ(parent.getChildByName("child2"), &child2);
	EXPECT_EQ(parent.getChildByName("incorrect_id"), nullptr);
	EXPECT_EQ(parent.getChildByName("incorrect_id.nested"), nullptr);
	EXPECT_EQ(child1.getChildByName("grandchild"), &grandchild);
	EXPECT_EQ(parent.getChildByName("child1.grandchild"), &grandchild);
	EXPECT_EQ(parent.getChildByName("child1.incorrect_id"), nullptr);
}

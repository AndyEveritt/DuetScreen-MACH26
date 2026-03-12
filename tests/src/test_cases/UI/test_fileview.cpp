/*
 * test_fileview.cpp
 *
 *  Created on: 2026-03-12
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Screens/File/FileView.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class TestFilePresenter : public TestSuite
{
  public:
	TestFilePresenter() {}
};

TEST(TestFilePresenter, MacroDisplayNameRemovesNumericPrefix)
{
	EXPECT_EQ(FilePresenter::getDisplayName("12_Preheat.g", FilePresenter::BaseFolder::MACROS), "Preheat.g");
	EXPECT_EQ(FilePresenter::getDisplayName("001_Home All.g", FilePresenter::BaseFolder::MACROS), "Home All.g");
	EXPECT_EQ(FilePresenter::getDisplayName("Preheat.g", FilePresenter::BaseFolder::MACROS), "Preheat.g");
	EXPECT_EQ(FilePresenter::getDisplayName("12Preheat.g", FilePresenter::BaseFolder::MACROS), "12Preheat.g");
	EXPECT_EQ(FilePresenter::getDisplayName("12_Preheat.g", FilePresenter::BaseFolder::GCODES), "12_Preheat.g");
}

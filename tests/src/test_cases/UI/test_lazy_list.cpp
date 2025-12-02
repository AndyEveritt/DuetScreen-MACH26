/*
 * test_lazy_list.cpp
 *
 *  Created on: 2025-12-02
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/List/LazyList.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class LazyLabel : public LazyObj<LvLabel>
{
  public:
	int32_t getSize() const override { return 30; }
	void update(LvLabel& obj) override { obj.setText(m_text); }

	void setText(const std::string& text) { m_text = text; }

  private:
	std::string m_text;
};

class TestLazyList : public UiTestSuite
{
  public:
	TestLazyList() {}
};

TEST_F(TestLazyList, Basic)
{
	LazyList<LazyLabel> list("lazy_list", screen);

	list.setSize(100, 100);
	list.setItemCount(10,
					  [](size_t index)
					  {
						  auto lazyLabel = std::make_unique<LazyLabel>();
						  lazyLabel->setText(fmt::format("Item {:d}", index));
						  return lazyLabel;
					  });

	EXPECT_EQUAL_SCREENSHOT("list/lazy_list_basic.png");
	EXPECT_EQ(list.getLazyItems().size(), 10);
	EXPECT_EQ(list.getVisibleItems().size(), 3);
	/**
	 * The capacity indicates whether the amount of visible items was ever set to 10 in which case there is no advantage
	 * over a regular list. This could happen when the list size is first calculated if the list container uses
	 * LV_SIZE_CONTENT.
	 */
	EXPECT_EQ(list.getVisibleItems().capacity(), 3);

	list.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	EXPECT_EQUAL_SCREENSHOT("list/lazy_list_basic_content_size.png");
	EXPECT_EQ(list.getLazyItems().size(), 10);
	EXPECT_EQ(list.getVisibleItems().size(), 10);
	EXPECT_EQ(list.getVisibleItems().capacity(), 10);
}

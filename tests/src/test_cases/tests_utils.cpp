#include <gtest/gtest.h>
#include <string>

#include "Debug.h"
#include "utils/utils.h"

TEST(Utils, BoundInt)
{
	EXPECT_EQ(utils::bound(5, 0, 10), 5);
	EXPECT_EQ(utils::bound(-1, 0, 10), 0);
	EXPECT_EQ(utils::bound(42, 0, 10), 10);
}

TEST(Utils, ReplaceSubstring)
{
	std::string s = "foo_bar_bar";
	utils::replaceSubstring(s, "bar", "baz");
	EXPECT_EQ(s, std::string("foo_baz_baz"));
}

TEST(Utils, SplitString)
{
	auto parts = utils::splitString("a,b,,c", ",");
	ASSERT_EQ(parts.size(), 3u);
	EXPECT_EQ(parts[0], std::string("a"));
	EXPECT_EQ(parts[1], std::string("b"));
	EXPECT_EQ(parts[2], std::string("c"));
}

TEST(Utils, FindInstance)
{
	size_t pos = utils::findInstance("abc abc abc", "abc", 2);
	// The second instance should begin at index 4 (0-based): "abc abc ..."
	EXPECT_EQ(pos, 5u); // function returns pos+1 after counting
}

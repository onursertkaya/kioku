#include "dstruct/buffer.h"
#include "dstruct/view.h"
#include <regex>
#include <vector>

#include "gtest/gtest.h"

using kioku::core::DynamicBuffer;
using kioku::core::StaticBuffer;
using kioku::core::View;

TEST(View, create)
{
    auto b = StaticBuffer<int, 10>{};

    auto b_view = createView(b);

    ASSERT_EQ(b_view.size(), b.size());
}

TEST(View, range)
{
    auto b = StaticBuffer<int, 10>{};
    b[0U] = 1;
    b[1U] = -2;
    b[2U] = 3;
    b[3U] = -4;
    b[4U] = 5;
    b[5U] = -6;
    b[6U] = 7;
    b[7U] = -8;
    b[8U] = 9;
    b[9U] = -10;

    auto b_view = createView(b);

    auto subview = b_view.range(3, 5);

    ASSERT_EQ(subview.at(0), -4);
    ASSERT_EQ(subview.at(1), 5);
    ASSERT_EQ(subview.size(), 2U);

    auto sub_subview = subview.range(0, 1);
    ASSERT_EQ(sub_subview.at(0), -4);
    ASSERT_EQ(sub_subview.size(), 1U);
}

TEST(View_DeathTest, invalid_range)
{
    auto b = DynamicBuffer<int>{4};
    auto b_view = createView(b);

    ASSERT_DEATH({ static_cast<void>(b_view.range(0, 0)); }, ".*");
}

TEST(View, range_based_loop)
{
    auto b = StaticBuffer<int, 10>{};
    auto b_view = createView(b);

    std::vector<int> src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::size_t ctr = 0;
    for (auto& elem : b_view)
    {
        elem = src[ctr];
        ctr++;
    }

    b[0U] = 1;
    b[1U] = 2;
    b[2U] = 3;
    b[3U] = 4;
    b[4U] = 5;
    b[5U] = 6;
    b[6U] = 7;
    b[7U] = 8;
    b[8U] = 9;
    b[9U] = 10;
}

TEST(DynamicView, create)
{
    auto b = StaticBuffer<int, 5>{};
    auto a = createDynamicView(b);

    ASSERT_EQ(a.size(), 0U);
}

TEST(DynamicView, push_back)
{
    auto b = StaticBuffer<int, 5>{};
    auto a = createDynamicView(b);

    a.push_back(1);
    a.push_back(2);

    ASSERT_EQ(a.size(), 2U);

    a.push_back(3);
    a.push_back(4);
    a.push_back(5);

    ASSERT_EQ(a.size(), 5U);

    testing::internal::CaptureStdout();
    a.push_back(-1);
    std::string const output = testing::internal::GetCapturedStdout();

    std::string const regex_statement{std::string{"\\[DynamicView\\] push_back\\(\\): "}};

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);
}

TEST(DynamicView, pop_back)
{
    auto b = StaticBuffer<int, 5>{};
    auto a = createDynamicView(b);

    ASSERT_EQ(a.size(), 0U);

    a.push_back(4);
    ASSERT_EQ(a.size(), 1U);

    a.pop_back();
    ASSERT_EQ(a.size(), 0U);

    testing::internal::CaptureStdout();
    a.pop_back();
    std::string const output = testing::internal::GetCapturedStdout();

    std::string const regex_statement{std::string{"\\[DynamicView\\] pop_back\\(\\): "}};

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);
}

TEST(DynamicView, at_access)
{
    auto b = StaticBuffer<int, 5>{};
    auto a = createDynamicView(b);

    for (int i = 4; i < 9; ++i)
    {
        a.push_back(i);
    }

    ASSERT_EQ(a.size(), 5U);

    ASSERT_EQ(a.at(0), 4);
    ASSERT_EQ(a.at(1), 5);
    ASSERT_EQ(a.at(2), 6);
    ASSERT_EQ(a.at(3), 7);
    ASSERT_EQ(a.at(4), 8);
}

#include "algo/sort.h"

#include "dstruct/static_array.h"
#include "gtest/gtest.h"

#include "util/rof.h"

using kioku::core::StaticArray;
using kioku::testing::RuleOfFiveHelper;

class TestSort : public ::testing::Test
{
 protected:
    static constexpr std::size_t size_1{10U};
    StaticArray<int, size_1> source_1{-1, 0, -2, 4, 12, -11, 0, -2, 25, -3};
    StaticArray<int, size_1> target_1{-11, -3, -2, -2, -1, 0, 0, 4, 12, 25};

    static constexpr std::size_t size_2{16U};
    StaticArray<int, size_2> source_2{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    StaticArray<int, size_2> target_2{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
};

TEST_F(TestSort, selection)
{
    StaticArray<int, size_1> sorted_1{source_1};
    kioku::algo::sort::selection(sorted_1);
    ASSERT_EQ(sorted_1.size(), target_1.size());

    for (uint32_t i{0U}; i < sorted_1.size(); ++i)
    {
        ASSERT_EQ(sorted_1[i], target_1[i]);
    }

    StaticArray<int, size_2> sorted_2{source_2};
    kioku::algo::sort::selection(sorted_2);
    ASSERT_EQ(sorted_2.size(), target_2.size());

    for (uint32_t i{0U}; i < sorted_2.size(); ++i)
    {
        ASSERT_EQ(sorted_2[i], target_2[i]);
    }
}

TEST_F(TestSort, insertion)
{
    StaticArray<int, size_1> sorted_1{source_1};
    kioku::algo::sort::insertion(sorted_1);
    ASSERT_EQ(sorted_1.size(), target_1.size());

    for (uint32_t i{0U}; i < sorted_1.size(); ++i)
    {
        ASSERT_EQ(sorted_1[i], target_1[i]);
    }

    StaticArray<int, size_2> sorted_2{source_2};
    kioku::algo::sort::insertion(sorted_2);
    ASSERT_EQ(sorted_2.size(), target_2.size());

    for (uint32_t i{0U}; i < sorted_2.size(); ++i)
    {
        ASSERT_EQ(sorted_2[i], target_2[i]);
    }
}

TEST_F(TestSort, bubble)
{
    StaticArray<int, size_1> sorted_1{source_1};
    kioku::algo::sort::bubble(sorted_1);
    ASSERT_EQ(sorted_1.size(), target_1.size());

    for (uint32_t i{0U}; i < sorted_1.size(); ++i)
    {
        ASSERT_EQ(sorted_1[i], target_1[i]);
    }

    StaticArray<int, size_2> sorted_2{source_2};
    kioku::algo::sort::bubble(sorted_2);
    ASSERT_EQ(sorted_2.size(), target_2.size());

    for (uint32_t i{0U}; i < sorted_2.size(); ++i)
    {
        ASSERT_EQ(sorted_2[i], target_2[i]);
    }
}

TEST_F(TestSort, merge)
{
    StaticArray<int, size_1> sorted_1{source_1};
    kioku::algo::sort::merge(sorted_1);
    ASSERT_EQ(sorted_1.size(), target_1.size());

    for (uint32_t i{0U}; i < sorted_1.size(); ++i)
    {
        ASSERT_EQ(sorted_1[i], target_1[i]);
    }

    StaticArray<int, size_2> sorted_2{source_2};
    kioku::algo::sort::merge(sorted_2);
    ASSERT_EQ(sorted_2.size(), target_2.size());

    for (uint32_t i{0U}; i < sorted_2.size(); ++i)
    {
        ASSERT_EQ(sorted_2[i], target_2[i]);
    }
}

#include "algo/search.h"

#include "dstruct/buffer.h"
#include "gtest/gtest.h"

using kioku::core::StaticBuffer;

TEST(TestSearch, linear)
{
    StaticBuffer<int, 5U> arr{-1, 0, 1, 2, 3};

    constexpr int item_to_find{2};
    auto res = kioku::algo::search::linear(arr, item_to_find);
    ASSERT_EQ(res.idx, 3U);
    ASSERT_EQ(res.val, item_to_find);
}

TEST(TestSearch, binary)
{
    StaticBuffer<int, 17U> arr{0, 2, 5, 15, 26, 29, 31, 37, 41, 45, 46, 47, 48, 71, 123, 522, 1502};

    StaticBuffer<int, 6U> items{2, 26, 37, 47, 71, 522};
    StaticBuffer<std::size_t, 6U> idxs{1UL, 4UL, 7UL, 11UL, 13UL, 15UL};

    for (std::size_t i{0U}; i < items.size(); ++i)
    {
        auto res = kioku::algo::search::binary(arr, items[i]);
        ASSERT_EQ(res.idx, idxs[i]);
        ASSERT_EQ(res.val, items[i]);
    }
}

TEST(TestSearch, find_max)
{
    StaticBuffer<int, 5U> arr{-1, 0, 1, 2, 3};

    auto res = kioku::algo::search::max(arr);
    ASSERT_EQ(res.idx, 4U);
    ASSERT_EQ(res.val, 3);
}

TEST(TestSearch, find_min)
{
    StaticBuffer<int, 5U> arr{-1, 0, 1, 2, 3};

    auto res = kioku::algo::search::min(arr);
    ASSERT_EQ(res.idx, 0U);
    ASSERT_EQ(res.val, -1);
}

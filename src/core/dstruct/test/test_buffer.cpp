#include "dstruct/buffer.h"

#include "gtest/gtest.h"
#include <array>

using kioku::core::DynamicBuffer;
using kioku::core::StaticBuffer;

TEST(DynamicBuffer, create)
{
    auto b = DynamicBuffer<int>{10};
    ASSERT_EQ(b.size(), 10U);
}

TEST(DynamicBuffer, create_from_container)
{
    auto b = DynamicBuffer<int>{std::array<int, 4>{4, 3, 2, 1}};
    ASSERT_EQ(b.size(), 4U);

    b[0U] = 4;
    b[1U] = 3;
    b[2U] = 2;
    b[3U] = 1;
}

TEST(DynamicBuffer_DeathTest, create_assert)
{
    ASSERT_DEATH({ static_cast<void>(DynamicBuffer<int>{0}); }, ".*");
}

TEST(DynamicBuffer, indexed_access)
{
    auto b = DynamicBuffer<int>{4};
    b[0U] = 10;
    b[1U] = 20;
    b[2U] = 30;
    b[3U] = 40;

    ASSERT_EQ(b[0U], 10);
    ASSERT_EQ(b[1U], 20);
    ASSERT_EQ(b[2U], 30);
    ASSERT_EQ(b[3U], 40);
}

TEST(DynamicBuffer, reallocation)
{
    auto b = DynamicBuffer<int>{4};
    b[0U] = 10;
    b[1U] = 20;
    b[2U] = 30;
    b[3U] = 40;

    b.reallocate(6U);

    ASSERT_EQ(b.size(), 6U);

    ASSERT_EQ(b[0U], 10);
    ASSERT_EQ(b[1U], 20);
    ASSERT_EQ(b[2U], 30);
    ASSERT_EQ(b[3U], 40);

    ASSERT_EQ(b[4U], int{});
    ASSERT_EQ(b[5U], int{});
}

TEST(DynamicBuffer_DeathTest, reallocation_assert)
{
    auto b = DynamicBuffer<int>{4};

    ASSERT_DEATH({ static_cast<void>(b.reallocate(2U)); }, ".*");
}

TEST(StaticBuffer, create)
{
    auto b = StaticBuffer<int, 10U>{};
    ASSERT_EQ(b.size(), 10U);
}

TEST(StaticBuffer, variadic_create)
{
    auto b = StaticBuffer<int, 10U>{5, 6, 7, 8, 1, 2, 3, 4};
    ASSERT_EQ(b.size(), 10U);

    ASSERT_EQ(b[0U], 5);
    ASSERT_EQ(b[1U], 6);
    ASSERT_EQ(b[2U], 7);
    ASSERT_EQ(b[3U], 8);
    ASSERT_EQ(b[4U], 1);
    ASSERT_EQ(b[5U], 2);
    ASSERT_EQ(b[6U], 3);
    ASSERT_EQ(b[7U], 4);

    ASSERT_EQ(b[8U], int{});
    ASSERT_EQ(b[9U], int{});
}

TEST(StaticBuffer, indexed_access)
{
    auto b = StaticBuffer<int, 4U>{};
    b[0U] = 10;
    b[1U] = 20;
    b[2U] = 30;
    b[3U] = 40;

    ASSERT_EQ(b[0U], 10);
    ASSERT_EQ(b[1U], 20);
    ASSERT_EQ(b[2U], 30);
    ASSERT_EQ(b[3U], 40);
}

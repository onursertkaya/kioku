#include "dstruct/buffer.h"

#include "gtest/gtest.h"

using kioku::core::DynamicBuffer;
using kioku::core::StaticBuffer;

TEST(DynamicBuffer, create)
{
    auto b = DynamicBuffer<int>{10};
    ASSERT_EQ(b.size(), 10U);
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

TEST(DynamicBuffer, iterator_access)
{
    auto b = DynamicBuffer<int>{4};

    ASSERT_EQ(b.begin() + std::ptrdiff_t{4U}, b.end());
}

TEST(DynamicBuffer, reset)
{
    auto b = DynamicBuffer<int>{4};

    b.reset();

    ASSERT_EQ(b.begin(), nullptr);
    ASSERT_EQ(b.begin(), b.end());
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

TEST(Static, copy_ctor)
{
    auto buffer = StaticBuffer<int, 3U>{};
    auto copy_assigned = StaticBuffer<int, 3U>{};

    buffer[0U] = 12;
    buffer[1U] = 13;
    buffer[2U] = 14;

    auto copy_constructed = buffer;
    copy_assigned = buffer;

    ASSERT_EQ(buffer.size(), 3U);
    ASSERT_EQ(copy_constructed.size(), 3U);
    ASSERT_EQ(copy_assigned.size(), 3U);

    ASSERT_EQ(buffer[0U], 12);
    ASSERT_EQ(copy_constructed[0U], 12);
    ASSERT_EQ(copy_assigned[0U], 12);

    ASSERT_EQ(buffer[1U], 13);
    ASSERT_EQ(copy_constructed[1U], 13);
    ASSERT_EQ(copy_assigned[1U], 13);

    ASSERT_EQ(buffer[2U], 14);
    ASSERT_EQ(copy_constructed[2U], 14);
    ASSERT_EQ(copy_assigned[2U], 14);

    ASSERT_NE(&buffer[0U], &copy_constructed[0U]);
    ASSERT_NE(&buffer[0U], &copy_assigned[0U]);
    ASSERT_NE(&copy_constructed[0U], &copy_assigned[0U]);
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

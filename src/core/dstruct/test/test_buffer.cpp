#include "dstruct/buffer.h"

#include "gtest/gtest.h"
#include <array>

using kioku::core::DynamicBuffer;
using kioku::core::StaticBuffer;

// TODO: find a more elegant solution to write the same tests for const instances.
// possibly via TYPED_TEST.

TEST(StaticBuffer, create)
{
    auto b = StaticBuffer<int, 10U>{};
    ASSERT_EQ(b.size(), 10U);

    auto const c_b = StaticBuffer<int, 5U>{};
    ASSERT_EQ(c_b.size(), 5U);
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

    auto const c_b = StaticBuffer<int, 5U>{5, 6, 1, 2};
    ASSERT_EQ(c_b.size(), 5U);

    ASSERT_EQ(c_b[0U], 5);
    ASSERT_EQ(c_b[1U], 6);
    ASSERT_EQ(c_b[2U], 1);
    ASSERT_EQ(c_b[3U], 2);

    ASSERT_EQ(c_b[4U], int{});
}

TEST(StaticBuffer, index_access)
{
    auto b = StaticBuffer<int, 4U>{};
    b[0U] = 10;
    b[1U] = 20;

    ASSERT_EQ(b[0U], 10);
    ASSERT_EQ(b[1U], 20);
}

TEST(DynamicBuffer, create)
{
    auto b = DynamicBuffer<int>{10};
    ASSERT_EQ(b.size(), 10U);
}

TEST(DynamicBuffer, create_from_container)
{
    // from rvalue
    auto b_from_rvalue = DynamicBuffer<int>{std::array<int, 4>{4, 3, 2, 1}};
    ASSERT_EQ(b_from_rvalue.size(), 4U);

    ASSERT_EQ(b_from_rvalue[0U], 4);
    ASSERT_EQ(b_from_rvalue[1U], 3);
    ASSERT_EQ(b_from_rvalue[2U], 2);
    ASSERT_EQ(b_from_rvalue[3U], 1);

    // from non-const lvalue
    std::array<int, 4U> some_array{5, 6, 7};
    auto b_from_lvalue = DynamicBuffer<int>{some_array};

    ASSERT_EQ(b_from_lvalue[0U], 5);
    ASSERT_EQ(b_from_lvalue[1U], 6);
    ASSERT_EQ(b_from_lvalue[2U], 7);
    ASSERT_EQ(b_from_lvalue[3U], int{});

    // from const lvalue
    std::array<int, 4U> const some_const_array{1, 2, 3};
    auto b_from_const_lvalue = DynamicBuffer<int>{some_const_array};

    ASSERT_EQ(b_from_const_lvalue[0U], 1);
    ASSERT_EQ(b_from_const_lvalue[1U], 2);
    ASSERT_EQ(b_from_const_lvalue[2U], 3);
    ASSERT_EQ(b_from_const_lvalue[3U], int{});

    // const instance from rvalue
    auto const c_b = DynamicBuffer<int>{std::array<int, 4>{4, 3, 2, 1}};
    ASSERT_EQ(c_b.size(), 4U);

    ASSERT_EQ(c_b[0U], 4);
    ASSERT_EQ(c_b[1U], 3);
    ASSERT_EQ(c_b[2U], 2);
    ASSERT_EQ(c_b[3U], 1);
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

    // TODO: this ctor does not make sense as the created object
    // does not support further index assignment. see the ctor
    // for details
    auto const c_b = DynamicBuffer<int>{3};
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

TEST(DynamicBuffer_DeathTest, reallocation_assert_2)
{
    auto b = DynamicBuffer<int>{2};

    ASSERT_DEATH({ static_cast<void>(b.reallocate(2U)); }, ".*");
}

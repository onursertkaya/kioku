#include <array>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>

#include "dstruct/static_array.h"

#include "gtest/gtest.h"

using kioku::core::StaticArray;

TEST(StaticArray, init_val_ctor)
{
    int const initialization_value{-8};
    auto a = StaticArray<int, 5U>::createFill(initialization_value);

    ASSERT_EQ(a.bufferCapacity(), 5U);
    ASSERT_EQ(a.size(), 0U);
    ASSERT_TRUE(a.isEmpty());
}

TEST(StaticArray, variadic_ctor)
{
    StaticArray<int, 5U> a{1, 2, 3};

    ASSERT_EQ(a.bufferCapacity(), 5U);
    ASSERT_EQ(a.size(), 3U);

    ASSERT_EQ(a[0U], 1);
    ASSERT_EQ(a[1U], 2);
    ASSERT_EQ(a[2U], 3);

    // expect values in the uninitialized memory region
    // to be equal to the default value of T.
    ASSERT_EQ(a[3U], int{});
    ASSERT_EQ(a[4U], int{});
}

TEST(StaticArray, copy_ctor_and_copy_assignment)
{
    StaticArray<int, 3U> a{1, 2, 3};

    StaticArray<int, 3U> b{a};

    ASSERT_EQ(b.bufferCapacity(), 3U);
    ASSERT_EQ(b.size(), 3U);

    ASSERT_EQ(b[0U], 1);
    ASSERT_EQ(b[1U], 2);
    ASSERT_EQ(b[2U], 3);

    StaticArray<int, 3U> c = a;

    ASSERT_EQ(c.bufferCapacity(), 3U);
    ASSERT_EQ(c.size(), 3U);

    ASSERT_EQ(c[0U], 1);
    ASSERT_EQ(c[1U], 2);
    ASSERT_EQ(c[2U], 3);
}

TEST(StaticArray, copy_ctor_with_fill_val)
{
    StaticArray<int, 5U> a{1, 2, 3};
    StaticArray<int, 5U> b{a, 100};

    ASSERT_EQ(b.bufferCapacity(), 5U);
    ASSERT_EQ(b.size(), 3U);

    ASSERT_EQ(b[0U], 1);
    ASSERT_EQ(b[1U], 2);
    ASSERT_EQ(b[2U], 3);

    ASSERT_EQ(b[3U], 100);
    ASSERT_EQ(b[4U], 100);
}

TEST(StaticArray, create_from)
{
    StaticArray<int, 3U> a{1, 2, 3};

    auto b = StaticArray<int, 5U>::createFrom(a, 100);

    ASSERT_EQ(b.bufferCapacity(), 5U);
    ASSERT_EQ(b.size(), 3U);

    ASSERT_EQ(b[0U], 1);
    ASSERT_EQ(b[1U], 2);
    ASSERT_EQ(b[2U], 3);

    ASSERT_EQ(b[3U], 100);
    ASSERT_EQ(b[4U], 100);
}

TEST(StaticArray, container_ctor)
{
    std::vector<float> const vec{1.F, 2.F, 3.F, 4.F, 5.F};

    StaticArray<float, 5> a{vec};
    StaticArray<float, 8> a_oversized{vec};
    StaticArray<float, 8> a_oversized_with_default_val{vec, 42.F};

    ASSERT_EQ(a[0U], 1.F);
    ASSERT_EQ(a[1U], 2.F);
    ASSERT_EQ(a[2U], 3.F);
    ASSERT_EQ(a[3U], 4.F);
    ASSERT_EQ(a[4U], 5.F);

    ASSERT_EQ(a_oversized[0U], 1.F);
    ASSERT_EQ(a_oversized[1U], 2.F);
    ASSERT_EQ(a_oversized[2U], 3.F);
    ASSERT_EQ(a_oversized[3U], 4.F);
    ASSERT_EQ(a_oversized[4U], 5.F);

    // expect values in the uninitialized memory region
    // to be equal to the default value of T.
    ASSERT_EQ(a_oversized[5U], float{});
    ASSERT_EQ(a_oversized[6U], float{});
    ASSERT_EQ(a_oversized[7U], float{});

    ASSERT_EQ(a_oversized_with_default_val[0U], 1.F);
    ASSERT_EQ(a_oversized_with_default_val[1U], 2.F);
    ASSERT_EQ(a_oversized_with_default_val[2U], 3.F);
    ASSERT_EQ(a_oversized_with_default_val[3U], 4.F);
    ASSERT_EQ(a_oversized_with_default_val[4U], 5.F);

    ASSERT_EQ(a_oversized_with_default_val[5U], 42.F);
    ASSERT_EQ(a_oversized_with_default_val[6U], 42.F);
    ASSERT_EQ(a_oversized_with_default_val[7U], 42.F);
}

TEST(StaticArray, oob_access_does_not_throw)
{
    StaticArray<int, 3U> a{1, 2, 3};

    ASSERT_EQ(a[0U], 1);
    ASSERT_EQ(a[1U], 2);
    ASSERT_EQ(a[2U], 3);

    testing::internal::CaptureStdout();
    ASSERT_EQ(a[4U], 3);
    std::string output = testing::internal::GetCapturedStdout();

    std::string const regex_statement{
        std::string{"\\[StaticArray\\] operator\\[\\]\\(\\): Out of bounds access at idx: 4"}};

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);
}

TEST(StaticArray, push_back_to_empty)
{
    auto a = StaticArray<int, 5U>::createFill();

    ASSERT_EQ(a.bufferCapacity(), 5U);
    ASSERT_EQ(a.size(), 0U);

    a.push_back(1);
    a.push_back(2);

    ASSERT_EQ(a.size(), 2U);

    a.push_back(3);
    a.push_back(4);
    a.push_back(5);

    ASSERT_EQ(a.bufferCapacity(), 5U);
    ASSERT_EQ(a.size(), 5U);

    testing::internal::CaptureStdout();
    a.push_back(-1);
    std::string const output = testing::internal::GetCapturedStdout();

    std::string const regex_statement{std::string{"\\[StaticArray\\] push_back\\(\\): "}};

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);
}

TEST(StaticArray, push_back_existing)
{
    StaticArray<int, 5U> a{1, 2};

    ASSERT_EQ(a.bufferCapacity(), 5U);
    ASSERT_EQ(a.size(), 2U);

    a.push_back(4);
    a.push_back(3);

    ASSERT_EQ(a.bufferCapacity(), 5U);
    ASSERT_EQ(a.size(), 4U);
}

TEST(StaticArray, pop_back)
{
    StaticArray<int, 5U> a{1, 2};

    ASSERT_EQ(a.bufferCapacity(), 5U);
    ASSERT_EQ(a.size(), 2U);

    a.pop_back();
    ASSERT_EQ(a.size(), 1U);
    a.pop_back();
    ASSERT_EQ(a.size(), 0U);

    testing::internal::CaptureStdout();
    a.pop_back();
    std::string const output = testing::internal::GetCapturedStdout();

    std::string const regex_statement{std::string{"\\[StaticArray\\] pop_back\\(\\): "}};

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);
}

TEST(StaticArray, begin_end)
{
    StaticArray<int, 5U> a{1, 2, 3};

    ASSERT_EQ(*a.begin(), 1);
    ASSERT_EQ(*(1U + a.begin()), 2);
    ASSERT_EQ(*(2U + a.begin()), 3);
    ASSERT_EQ(*(3U + a.begin()), int{});
    ASSERT_EQ(*(4U + a.begin()), int{});

    // last valid element is at idx 2, so we expect end to be at idx 3.
    ASSERT_TRUE((3U + a.begin()) == a.end());

    auto empty = StaticArray<int, 5U>::createFill(-1);
    ASSERT_TRUE(empty.begin() == empty.end());
}

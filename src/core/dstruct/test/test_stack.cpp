#include "dstruct/stack.h"

#include "gtest/gtest.h"

using kioku::core::Stack;

TEST(Stack, create)
{
    auto s = Stack<int>{};
    ASSERT_EQ(s.size(), 0U);
}

TEST(Stack, push)
{
    auto s = Stack<int>{};

    s.push(4);

    ASSERT_EQ(s.size(), 1U);

    s.push(5);
    s.push(6);

    ASSERT_EQ(s.size(), 3U);
}

TEST(Stack, pop)
{
    auto s = Stack<int>{};

    s.push(4);

    ASSERT_EQ(s.size(), 1U);

    s.push(5);
    s.push(6);

    ASSERT_EQ(s.size(), 3U);

    auto popped = s.pop();

    ASSERT_EQ(popped, 6);

    popped = s.pop();
    ASSERT_EQ(popped, 5);

    popped = s.pop();
    ASSERT_EQ(popped, 4);

    ASSERT_EQ(s.size(), 0U);

    testing::internal::CaptureStdout();
    popped = s.pop();
    std::string const output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "Empty stack.\n");
    ASSERT_EQ(popped, int{});
}
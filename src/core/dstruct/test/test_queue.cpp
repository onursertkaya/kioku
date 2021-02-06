#include "dstruct/queue.h"

#include "gtest/gtest.h"

using kioku::core::Queue;

TEST(Queue, create)
{
    auto s = Queue<int>{};
    ASSERT_EQ(s.size(), 0U);
}

TEST(Queue, push)
{
    auto s = Queue<int>{};

    s.push(4);

    ASSERT_EQ(s.size(), 1U);

    s.push(5);
    s.push(6);

    ASSERT_EQ(s.size(), 3U);
}

TEST(Queue, pop)
{
    auto s = Queue<int>{};

    s.push(4);

    ASSERT_EQ(s.size(), 1U);

    s.push(5);
    s.push(6);

    ASSERT_EQ(s.size(), 3U);

    auto popped = s.pop();

    ASSERT_EQ(popped, 4);

    popped = s.pop();
    ASSERT_EQ(popped, 5);

    popped = s.pop();
    ASSERT_EQ(popped, 6);

    ASSERT_EQ(s.size(), 0U);

    testing::internal::CaptureStdout();
    popped = s.pop();
    std::string const output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "Empty queue.\n");
    ASSERT_EQ(popped, int{});
}
#include "util/assert.h"

#include <regex>

#include "gtest/gtest.h"

TEST(assert_DeathTest, create_assert)
{
    ASSERT_DEATH({ kioku::core::KIOKU_ASSERT(3 > 5); }, ".*Assertion.*3 > 5");
}

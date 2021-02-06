#include <array>
#include <iostream>
#include <numeric>
#include <regex>

#include "math/vec_n.h"

#include "util/rof.h"
#include "gtest/gtest.h"

using kioku::core::vecN;
using kioku::testing::RuleOfFiveHelper;

TEST(VecN, copy_ctors_and_access)
{
    vecN<float, 3U> a{1.0F, 2.0F, 3.0F};

    ASSERT_FLOAT_EQ(a[0U], 1.0F);
    ASSERT_FLOAT_EQ(a[1U], 2.0F);
    ASSERT_FLOAT_EQ(a[2U], 3.0F);

    constexpr uint32_t elems{10U};
    vecN<float, elems> b(-2.0F);

    for (uint32_t idx{0U}; idx < elems; ++idx)
    {
        ASSERT_FLOAT_EQ(b[idx], -2.0F);
    }
}

TEST(VecN, access_assign)
{
    vecN<float, 3U> a{1.0F, 2.0F, 3.0F};

    ASSERT_FLOAT_EQ(a[0U], 1.0F);
    ASSERT_FLOAT_EQ(a[1U], 2.0F);
    ASSERT_FLOAT_EQ(a[2U], 3.0F);

    a[0U] = -1.0F;
    a[1U] = -2.0F;
    a[2U] = -3.0F;

    ASSERT_FLOAT_EQ(a[0U], -1.0F);
    ASSERT_FLOAT_EQ(a[1U], -2.0F);
    ASSERT_FLOAT_EQ(a[2U], -3.0F);
}

TEST(VecN, move_ctor)
{
    using vecRof = RuleOfFiveHelper<vecN<float, 1U>>;

    testing::internal::CaptureStdout();
    vecRof::move();
    std::string output = testing::internal::GetCapturedStdout();

    std::string regex_statement{std::string{"(\\w*vecN\\w*)"} + std::string{"\\s"} + "(" +
                                std::string{vecRof::moveCtor} + ")"};
    // fixme, why does appending $ not work?

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);

    vecN<float, 5U> b{vecN<float, 5U>{5.1F, 4.1F, 3.1F, 2.1F, 1.1F}};

    ASSERT_FLOAT_EQ(b[0U], 5.1F);
    ASSERT_FLOAT_EQ(b[1U], 4.1F);
    ASSERT_FLOAT_EQ(b[2U], 3.1F);
    ASSERT_FLOAT_EQ(b[3U], 2.1F);
    ASSERT_FLOAT_EQ(b[4U], 1.1F);
}

TEST(VecN, elemwise_ops_and_access)
{
    vecN<float, 3U> a{1.0F, 2.0F, 3.0F};

    auto b{a + 3.0F};

    ASSERT_FLOAT_EQ(b[0U], 4.0F);
    ASSERT_FLOAT_EQ(b[1U], 5.0F);
    ASSERT_FLOAT_EQ(b[2U], 6.0F);

    auto c{b * -2.0F};

    ASSERT_FLOAT_EQ(c[0U], -8.0F);
    ASSERT_FLOAT_EQ(c[1U], -10.0F);
    ASSERT_FLOAT_EQ(c[2U], -12.0F);

    auto d{c - 1.5F};

    ASSERT_FLOAT_EQ(d[0U], -9.50F);
    ASSERT_FLOAT_EQ(d[1U], -11.5F);
    ASSERT_FLOAT_EQ(d[2U], -13.5F);

    auto e{d / 10.0F};

    ASSERT_FLOAT_EQ(e[0U], -0.950F);
    ASSERT_FLOAT_EQ(e[1U], -1.15F);
    ASSERT_FLOAT_EQ(e[2U], -1.35F);

    vecN<float, 3U> f{-0.95F, -1.15F, -1.35F};

    ASSERT_TRUE(e == f);
}

TEST(VecN, dot_product)
{
    vecN<float, 3U> a{1.0F, -5.3F, 3.0F};
    vecN<float, 3U> b{-2.3F, 2.0F, 4.6F};

    float c{a.dot(b)};
    float d{b.dot(a)};

    float const e{0.9F};

    EXPECT_NEAR(e, c, 1E-5F);
    EXPECT_NEAR(e, d, 1E-5F);
}

TEST(VecN, norm)
{
    vecN<int, 3U> a{3, 4, 12};
    ASSERT_FLOAT_EQ(a.norm(), 13.0F);
}

TEST(VecN, sum)
{
    vecN<float, 3U> a{3.1F, -1.1F, 4.0F};
    ASSERT_FLOAT_EQ(a.sum(), 6.0F);
}

TEST(VecN, create_random)
{
    constexpr float kExpectedMean{0.3F};
    constexpr float kExpectedStddev{0.1F};
    auto ran{vecN<float, 100U>::random(kExpectedMean, kExpectedStddev)};

    float const mean{ran.sum() / ran.height};

    float var{0.0F};
    for (uint32_t idx{0U}; idx < ran.height; ++idx)
    {
        var += std::pow(mean - ran[idx], 2.0F);
    }
    var /= ran.height;

    EXPECT_NEAR(mean, kExpectedMean, 1E-2F);
    EXPECT_NEAR(std::sqrt(var), kExpectedStddev, 1E-2F);
}

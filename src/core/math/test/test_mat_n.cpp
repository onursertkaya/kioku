#include <array>
#include <iostream>
#include <numeric>

#include "math/mat_n.h"

#include "gtest/gtest.h"

using kioku::core::matN;
using kioku::core::vecN;

TEST(MatN, inv_2x2)
{
    matN<float, 2U, 2U> a{{1.2F, 3.0F}, {1.0F, -4.2F}};

    auto a_inv{a.inv()};

    ASSERT_FLOAT_EQ(a_inv[0U][0U], 0.52238806F);
    ASSERT_FLOAT_EQ(a_inv[0U][1U], 0.12437811F);
    ASSERT_FLOAT_EQ(a_inv[1U][0U], 0.37313433F);
    ASSERT_FLOAT_EQ(a_inv[1U][1U], -0.14925373F);
}

TEST(MatN_DeathTest, inv_2x2_zero_det)
{
    matN<float, 2U, 2U> a{{1.0F, 2.0F}, {-2.0F, -4.0F}};

    ASSERT_DEATH({ static_cast<void>(a.inv()); }, ".*");
}

TEST(MatN, brace_init_ctor_and_access)
{
    matN<float, 3U, 3U> a{{1.2F, 0.0F, 1.1F}, {0.0F, -1.2F, 5.5F}, {4.0F, -4.0F, 4.0F}};

    ASSERT_FLOAT_EQ(a[0U][0U], 1.2F);
    ASSERT_FLOAT_EQ(a[0U][1U], 0.0F);
    ASSERT_FLOAT_EQ(a[0U][2U], 1.1F);

    ASSERT_FLOAT_EQ(a[1U][0U], 0.0F);
    ASSERT_FLOAT_EQ(a[1U][1U], -1.2F);
    ASSERT_FLOAT_EQ(a[1U][2U], 5.5F);

    ASSERT_FLOAT_EQ(a[2U][0U], 4.0F);
    ASSERT_FLOAT_EQ(a[2U][1U], -4.0F);
    ASSERT_FLOAT_EQ(a[2U][2U], 4.0F);

    matN<float, 2U, 2U> const b{{1.2F, 0.0F}, {0.0F, -1.2F}};

    ASSERT_EQ(b[0U], (vecN<float, 2U>{1.2F, 0.0F}));
    ASSERT_EQ(b[1U], (vecN<float, 2U>{0.0F, -1.2F}));
}

TEST(MatN, value_init_and_access)
{
    constexpr uint32_t kSize{20U};
    matN<float, kSize, kSize> a{-1.0F};  // 20x20

    ASSERT_EQ(a.width, kSize);
    ASSERT_EQ(a.height, kSize);
    for (uint32_t idx{0U}; idx < kSize; ++idx)
    {
        ASSERT_FLOAT_EQ(a[0U][idx], -1.0F);
    }
}

TEST(MatN, copy_move_ctor_assignment)
{
    matN<float, 3U, 3U> a{{1.2F, 0.0F, 1.1F}, {0.0F, -1.2F, 5.5F}, {4.0F, -4.0F, 4.0F}};

    auto b = a;
    matN<float, 3U, 3U> c{a};
    matN<float, 3U, 3U> d{
        matN<float, 3U, 3U>{{1.2F, 0.0F, 1.1F}, {0.0F, -1.2F, 5.5F}, {4.0F, -4.0F, 4.0F}}};

    matN<float, 3U, 3U> e =
        matN<float, 3U, 3U>{{1.2F, 0.0F, 1.1F}, {0.0F, -1.2F, 5.5F}, {4.0F, -4.0F, 4.0F}};

    ASSERT_FLOAT_EQ(a[0U][0U], 1.2F);
    ASSERT_FLOAT_EQ(a[0U][1U], 0.0F);
    ASSERT_FLOAT_EQ(a[0U][2U], 1.1F);
    ASSERT_FLOAT_EQ(a[1U][0U], 0.0F);
    ASSERT_FLOAT_EQ(a[1U][1U], -1.2F);
    ASSERT_FLOAT_EQ(a[1U][2U], 5.5F);
    ASSERT_FLOAT_EQ(a[2U][0U], 4.0F);
    ASSERT_FLOAT_EQ(a[2U][1U], -4.0F);
    ASSERT_FLOAT_EQ(a[2U][2U], 4.0F);

    for (uint32_t x{0U}; x < 3U; ++x)
    {
        for (uint32_t y{0U}; y < 3U; ++y)
        {
            ASSERT_FLOAT_EQ(a[x][y], b[x][y]);
            ASSERT_FLOAT_EQ(a[x][y], c[x][y]);
            ASSERT_FLOAT_EQ(a[x][y], d[x][y]);
            ASSERT_FLOAT_EQ(a[x][y], e[x][y]);
        }
    }
}

TEST(MatN, mat_vec_product)
{
    matN<float, 3U, 2U> mat{{1.2F, -1.6F}, {-5.3F, 4.01F}, {10.0F, 1.1F}};  // 3x2

    vecN<float, 2U> vec{0.45F, 2.1F};  // 1x2

    auto out_mat{mat.dot(vec)};  // 3x1

    ASSERT_FLOAT_EQ(out_mat[0U][0U], -2.82F);
    ASSERT_FLOAT_EQ(out_mat[1U][0U], 6.036F);
    ASSERT_FLOAT_EQ(out_mat[2U][0U], 6.81F);
}

TEST(MatN, matrix_product)
{
    // 3x2
    matN<float, 3U, 2U> first{{1.0F, 2.0F}, {3.0F, 4.0F}, {5.0F, 6.0F}};

    // 2x2
    matN<float, 2U, 2U> second{{1.0F, -1.0F}, {-2.0F, 4.0F}};

    // 3x2
    matN<float, 3U, 2U> expected{{-3.0F, 7.0F}, {-5.0F, 13.0F}, {-7.0F, 19.0F}};

    auto res = first.dot(second);

    ASSERT_EQ(res.width, expected.width);
    ASSERT_EQ(res.height, expected.height);

    for (uint32_t r{0U}; r < res.height; ++r)
    {
        for (uint32_t c{0U}; c < res.width; ++c)
        {
            ASSERT_FLOAT_EQ(res[r][c], expected[r][c]);
        }
    }
}

TEST(MatN, transpose)
{
    matN<float, 3U, 2U> a{{1.0F, 2.0F}, {3.0F, 3.0F}, {5.0F, 6.0F}};

    auto a_T{a.transpose()};

    ASSERT_EQ(a.height, a_T.width);
    ASSERT_EQ(a.width, a_T.height);

    ASSERT_FLOAT_EQ(a[0U][0U], a_T[0U][0U]);
    ASSERT_FLOAT_EQ(a[0U][1U], a_T[1U][0U]);
    ASSERT_FLOAT_EQ(a[1U][0U], a_T[0U][1U]);
    ASSERT_FLOAT_EQ(a[1U][1U], a_T[1U][1U]);
    ASSERT_FLOAT_EQ(a[2U][0U], a_T[0U][2U]);
    ASSERT_FLOAT_EQ(a[2U][1U], a_T[1U][2U]);
}

#include "activation/activation.h"

#include "math/mat_n.h"
#include "math/vec_n.h"
#include "gtest/gtest.h"

namespace kioku
{

TEST(ActivationsTest, ReluTest)
{
    constexpr uint32_t kLen{6U};

    core::vecN<float, kLen> truth{0.8F, 0.0F, 32.2F, 1E-6F, 0.0F, 0.0F};

    core::matN<float, 1U, kLen> prediction{0.0F};

    prediction[0U][0U] = 0.8F;
    prediction[0U][1U] = 0.0F;
    prediction[0U][2U] = 32.2F;
    prediction[0U][3U] = 1E-6F;
    prediction[0U][4U] = -1E-6F;
    prediction[0U][5U] = -4.0F;

    auto relud = ml::relu(prediction);

    ASSERT_TRUE(truth == relud);
}

TEST(ActivationsTest, SoftmaxSmallLogits)
{
    constexpr uint32_t kLen{10U};
    core::matN<float, 1U, kLen> prediction{0.0F};

    // obtained with scipy.special.softmax
    core::vecN<float, kLen> truth{0.1811303F,  0.09550863F, 0.09361744F, 0.09268593F, 0.09176369F,
                                  0.09085062F, 0.08994664F, 0.08816558F, 0.08816558F, 0.08816558F};

    prediction[0U][0U] = 0.72F;
    prediction[0U][1U] = 0.08F;
    prediction[0U][2U] = 0.06F;
    prediction[0U][3U] = 0.05F;
    prediction[0U][4U] = 0.04F;
    prediction[0U][5U] = 0.03F;
    prediction[0U][6U] = 0.02F;
    prediction[0U][7U] = 0.0F;
    prediction[0U][8U] = 0.0F;
    prediction[0U][9U] = 0.0F;

    auto softmaxed = ml::softmax(prediction);

    ASSERT_FLOAT_EQ(softmaxed.sum(), 1.0F);

    ASSERT_TRUE(truth == softmaxed);
}

TEST(ActivationsTest, SoftmaxNegativeAndPositiveLogits)
{
    constexpr uint32_t kLen{5U};
    core::matN<float, 1U, kLen> prediction{0.0F};

    // obtained with scipy.special.softmax
    core::vecN<float, kLen> truth{1.26721598E-22F, 7.50929514E-14F, 9.92901086E-06F,
                                  1.98401087E-02F, 9.80149962E-01F};
    prediction[0U][0U] = -45.0F;
    prediction[0U][1U] = -24.8F;
    prediction[0U][2U] = -6.1F;
    prediction[0U][3U] = 1.5F;
    prediction[0U][4U] = 5.4F;

    auto softmaxed = ml::softmax(prediction);

    ASSERT_FLOAT_EQ(softmaxed.sum(), 1.0F);

    ASSERT_TRUE(truth == softmaxed);
}

}  // namespace kioku

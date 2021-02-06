#ifndef SRC_PROJECTS_LINEAR_REGRESSION_LOSS_INCLUDE_LOSS_LOSS_H
#define SRC_PROJECTS_LINEAR_REGRESSION_LOSS_INCLUDE_LOSS_LOSS_H

// NOLINTNEXTLINE -- cmath has a bug, expf is not there.
#include <math.h>

#include "math/mat_n.h"

namespace kioku
{
namespace ml
{

template <typename T, uint32_t len>
core::vecN<T, len> l1_dist(core::matN<T, 1U, len> const& prediction,
                           core::matN<T, 1U, len> const& truth)
{
    auto diff{prediction.row(0U) - truth.row(0U)};

    for (uint32_t idx{0U}; idx < len; ++idx)
    {
        diff[idx] = std::abs(diff[idx]);
    }
    return diff;
}

// fixme: for some reason, it gets stuck with L1 loss.
template <typename T, uint32_t len>
core::matN<T, 1U, len> l1_dist_derivative(core::matN<T, 1U, len> const& prediction,
                                          core::matN<T, 1U, len> const& truth)
{
    // truth is not needed, however keep it as a param for the sake of
    // interface.
    static_cast<void>(truth);

    // to prevent returning a derivative at exactly 0.0F, give a positive nudge.
    core::matN<T, 1U, len> derivative(0.0F);

    for (uint32_t idx{0U}; idx < len; ++idx)
    {
        auto const& p{prediction.row(0)[idx]};
        derivative.row(0U)[idx] = p > 1E-6F ? 1.0F : -1.0F;
    }
    return derivative;
}

template <typename T, uint32_t len>
core::vecN<T, len> l2_dist(core::matN<T, 1U, len> const& prediction,
                           core::matN<T, 1U, len> const& truth)
{
    auto diff{prediction.row(0U) - truth.row(0U)};

    for (uint32_t idx{0U}; idx < len; ++idx)
    {
        diff[idx] = std::pow(diff[idx], 2.0F);
    }
    return diff;
}

template <typename T, uint32_t len>
core::matN<T, 1U, len> l2_dist_derivative(core::matN<T, 1U, len> const& prediction,
                                          core::matN<T, 1U, len> const& truth)
{
    auto derivative{prediction - truth};

    return derivative * static_cast<T>(2.0F);
}

// template <typename T, uint32_t len>
// auto logistic(
//     core::matN<T, 1U, len> const& prediction,
//     core::matN<T, 1U, len> const& truth)
// {
//     1 / (1 + std::exp())
// }

// template <typename T, uint32_t len>
// auto logistic_derivative(
//     core::matN<T, 1U, len> const& prediction,
//     core::matN<T, 1U, len> const& truth)
// {

// }

// template <typename T, uint32_t len>
// auto categorical_cross_entropy(core::matN<T, 1U, len> const& prediction,
//                                core::matN<T, 1U, len> const& truth)
// {

// }

}  // namespace ml
}  // namespace kioku

#endif  // SRC_PROJECTS_LINEAR_REGRESSION_LOSS_INCLUDE_LOSS_LOSS_H

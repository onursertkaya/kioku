#ifndef SRC_PROJECTS_LINEAR_REGRESSION_ACTIVATION_INCLUDE_ACTIVATION_ACTIVATION_H
#define SRC_PROJECTS_LINEAR_REGRESSION_ACTIVATION_INCLUDE_ACTIVATION_ACTIVATION_H

#include "math/mat_n.h"

namespace kioku
{
namespace ml
{

template <typename T, uint32_t len>
auto relu(core::matN<T, 1U, len> const& prediction)
{
    auto rel{prediction.row(0U)};

    for (uint32_t idx{0U}; idx < len; ++idx)
    {
        rel[idx] = rel[idx] > static_cast<T>(0.0F) ? rel[idx] : static_cast<T>(0.0F);
    }

    return rel;
}

template <typename T, uint32_t len>
auto relu_derivative(core::matN<T, 1U, len> const& prediction)
{
    auto relud{prediction.row(0U)};

    for (uint32_t idx{0U}; idx < len; ++idx)
    {
        relud[idx] =
            relud[idx] > static_cast<T>(0.0F) ? static_cast<T>(1.0F) : static_cast<T>(0.0F);
    }

    return relud;
}

template <typename T, uint32_t len>
auto softmax(core::matN<T, 1U, len> const& prediction)
{
    auto e_raised{prediction.row(0U)};

    e_raised -= e_raised.maxVal();  // for numeric stability

    for (uint32_t idx{0U}; idx < len; ++idx)
    {
        e_raised[idx] = expf(e_raised[idx]);
    }

    e_raised /= e_raised.sum();

    return e_raised;
}

template <typename T, uint32_t len>
auto softmax_derivative(core::matN<T, 1U, len> const& prediction)
{
    auto e_raised{prediction.row(0U)};

    e_raised -= e_raised.maxVal();  // for numeric stability

    for (uint32_t idx{0U}; idx < len; ++idx)
    {
        e_raised[idx] = expf(e_raised[idx]);
    }

    e_raised /= e_raised.sum();

    return e_raised;
}

}  // namespace ml
}  // namespace kioku

#endif  // SRC_PROJECTS_LINEAR_REGRESSION_ACTIVATION_INCLUDE_ACTIVATION_ACTIVATION_H

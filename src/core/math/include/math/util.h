#ifndef SRC_CORE_MATH_INCLUDE_MATH_UTIL_H
#define SRC_CORE_MATH_INCLUDE_MATH_UTIL_H

#include <cmath>
#include <type_traits>

namespace kioku
{
namespace math
{

constexpr float kEps{1E-4};

template <typename T>
inline std::enable_if_t<std::is_floating_point_v<T>, bool> isNear(T first, T second, T eps = kEps)
{
    return std::abs(first - second) < eps;
}

}  // namespace math
}  // namespace kioku

#endif  // SRC_CORE_MATH_INCLUDE_MATH_UTIL_H

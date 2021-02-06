#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_UTIL_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_UTIL_H

#include <cmath>
#include <type_traits>

namespace kioku
{
namespace core
{
template <std::size_t idx, typename Buffer, typename FirstT>
constexpr void fill(Buffer& target, FirstT const first)
{
    target[idx] = first;
}

template <std::size_t idx, typename Buffer, typename FirstT, typename... RestTs>
constexpr void fill(Buffer& target, FirstT const first, RestTs const... rest)
{
    // https://stackoverflow.com/questions/67231768/variadic-template-functions-where-to-put-base-case
    target[idx] = first;
    fill<idx + 1U>(target, rest...);
}

}  // namespace core
}  // namespace kioku

#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_UTIL_H

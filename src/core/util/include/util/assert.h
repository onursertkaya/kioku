#ifndef SRC_CORE_UTIL_INCLUDE_UTIL_ASSERT_H
#define SRC_CORE_UTIL_INCLUDE_UTIL_ASSERT_H

#include <cassert>
#include <iostream>

namespace kioku
{
namespace core
{
namespace color
{
constexpr char const* kReset{"\033[0m"};
constexpr char const* kBold{"\033[1m"};
constexpr char const* kRed{"\033[31m"};
constexpr char const* kGreen{"\033[32m"};
constexpr char const* kYellow{"\033[33m"};
constexpr char const* kBlue{"\033[34m"};
}  // namespace color

template <typename T>
inline void _kiokuAssert(T expr, char const* filename, char const* function, std::size_t const line,
                         char const* expression)
{
    static_assert(std::is_same_v<T, bool>, "Expression must evaluate to a boolean.");
    using color::kBold;
    using color::kRed;
    using color::kReset;
    using color::kYellow;
    if (!expr)
    {
        std::cerr << kBold << kRed << "[ " << filename << " ]\n"
                  << kYellow << "\tAssertion @ " << function << "()#" << line << ": " << expression
                  << kReset << std::endl;
        std::abort();
        // todo, investigate exit, quick_exit, abort, terminate
    }
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define KIOKU_ASSERT(expr) _kiokuAssert(expr, __FILE__, __func__, __LINE__, #expr)

}  // namespace core
}  // namespace kioku
#endif  // SRC_CORE_UTIL_INCLUDE_UTIL_ASSERT_H

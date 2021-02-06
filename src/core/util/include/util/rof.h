#ifndef SRC_CORE_UTIL_INCLUDE_UTIL_ROF_H
#define SRC_CORE_UTIL_INCLUDE_UTIL_ROF_H

#include <iostream>

namespace kioku
{
namespace testing
{
template <typename Any>
class RuleOfFiveHelper
{
 public:
    static constexpr char const* cTor{"Constructor"};
    static constexpr char const* dTor{"Destructor"};
    static constexpr char const* copyCtor{"Copy Constructor"};
    static constexpr char const* moveCtor{"Move Constructor"};
    static constexpr char const* copyAssignment{"Copy Assignment"};
    static constexpr char const* moveAssignment{"Move Assignment"};

    static void construct()
    {
        Any a;
        static_cast<void>(a);

        std::cout << name() << " " << cTor << std::endl;
    }

    static void copyConstruct(Any const& a)
    {
        Any b{a};
        static_cast<void>(b);

        std::cout << name() << " " << copyCtor << std::endl;
    }

    static void move()
    {
        Any a{Any{}};
        static_cast<void>(a);

        std::cout << name() << " " << moveCtor << std::endl;
    }

    static char const* name() { return typeid(Any).name(); }
};
}  // namespace testing
}  // namespace kioku

#endif  // SRC_CORE_UTIL_INCLUDE_UTIL_ROF_H

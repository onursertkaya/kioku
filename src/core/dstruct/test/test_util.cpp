#include "dstruct/util.h"
#include "gtest/gtest.h"

template <typename T, std::size_t n>
class DummyArray
{
 public:
    template <typename... Ts>
    explicit constexpr DummyArray(Ts const... init_vals)
    {
        static_assert(sizeof...(Ts) == n);
        kioku::core::fill<0U>(data_, init_vals...);
    }

    T operator[](std::size_t const idx) { return data_[idx]; }

 private:
    T data_[n]{};
};

TEST(util, fill_static_array)
{
    DummyArray<int, 3U> arr{1, 2, 3};

    ASSERT_EQ(arr[0U], 1);
    ASSERT_EQ(arr[1U], 2);
    ASSERT_EQ(arr[2U], 3);
}

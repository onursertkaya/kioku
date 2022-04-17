#ifndef SRC_CORE_MATH_INCLUDE_MATH_VEC_N_H
#define SRC_CORE_MATH_INCLUDE_MATH_VEC_N_H

#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>

#include "math/util.h"
#include "util/assert.h"

namespace kioku
{
namespace core
{
template <typename T, std::uint32_t N>
class vecN
{
    static_assert(std::is_arithmetic_v<T>);

 public:
    static constexpr uint32_t height{N};
    using value_type = T;

    // kioku-TIL
    // Before adding this ctor, matN was calling the initializer_list
    // constructor wih a list size of zero. This happened in hindsight,
    // which caused a bug. Take-away: always have the default ctor.
    // fixme, no longer relevant.
    vecN() = default;

    // Initialize with a fixed value.
    explicit vecN(T const init_val) { std::fill_n(data_, N, static_cast<T>(init_val)); }

    // Variadically initialize from arbitrary elements.
    template <typename... Ts, uint32_t num_elems = sizeof...(Ts)>
    explicit vecN(Ts&&... elems) :
        data_{std::forward<Ts>(elems)...}
    {
        static_assert(num_elems == N);
        // https://stackoverflow.com/questions/37139379/stdinitializer-list-vs-variadic-templates
    }

    // Initialize from C-array
    explicit vecN(T (&elems)[N])
    {
        for (uint32_t i{0U}; i < N; ++i)
        {
            data_[i] = elems[i];
        }
    }

    // Copy ctor (1/5)
    vecN(vecN const& other)
    {
        for (uint32_t r{0U}; r < height; ++r)
        {
            data_[r] = other[r];
        }
    }

    // Move ctor (2/5)
    vecN(vecN&& other) noexcept
    {
        // static_assert(height == other.height);
        std::swap(data_, other.data_);
    }

    // Copy Assignment (3/5)
    vecN& operator=(vecN const& other)
    {
        for (uint32_t r{0U}; r < height; ++r)
        {
            data_[r] = other[r];
        }
        return *this;
    }

    // Move Assignment (4/5)
    vecN& operator=(vecN&& other) noexcept
    {
        std::swap(data_, other.data_);
        return *this;
    }

    // Destructor (5/5)
    ~vecN() = default;

    static vecN random(T const mean, T const stddev, uint32_t const seed = 42U)
    {
        vecN retval{};
        std::mt19937 engine{seed};
        std::normal_distribution<T> normal_dist{mean, stddev};

        for (uint32_t r{0U}; r < height; ++r)
        {
            retval[r] = normal_dist(engine);
        }
        return retval;
    }

    // =================================================================

    bool operator==(vecN const& other) const
    {
        bool const default_retval{false};

        bool all_same{true};
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            // fixme, enable if for float/double/int
            all_same &= math::isNear(data_[idx], other[idx]);
        }
        return default_retval || all_same;
    }

    // element-wise addition
    vecN operator+(vecN const& other) const
    {
        vecN retval(static_cast<T>(0.0F));
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            retval[idx] = data_[idx] + other[idx];
        }
        return retval;
    }

    // element-wise self addition
    vecN& operator+=(vecN const& other)
    {
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            data_[idx] = data_[idx] + other[idx];
        }
        return *this;
    }

    // element-wise self subtraction
    vecN& operator-=(vecN const& other)
    {
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            data_[idx] = data_[idx] - other[idx];
        }
        return *this;
    }

    // element-wise subtraction
    vecN operator-(vecN const& other) const
    {
        vecN retval(static_cast<T>(0.0F));
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            retval[idx] = data_[idx] - other[idx];
        }
        return retval;
    }

    // elementwise multiplication
    vecN operator*(vecN const& other) const
    {
        vecN retval(static_cast<T>(0.0F));
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            retval[idx] = data_[idx] * other[idx];
        }
        return retval;
    }

    // elementwise division
    vecN operator/(vecN const& other) const
    {
        vecN retval(static_cast<T>(0.0F));
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            retval[idx] = data_[idx] / other[idx];
        }
        return retval;
    }

    // element-wise self division
    vecN& operator/=(vecN const& other)
    {
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            data_[idx] = data_[idx] / other[idx];
        }
        return *this;
    }

    // broadcasting
    vecN operator+(T const val) const { return *this + vecN(val); }
    vecN operator-(T const val) const { return *this - vecN(val); }
    vecN operator*(T const val) const { return *this * vecN(val); }
    vecN operator/(T const val) const { return *this / vecN(val); }

    vecN& operator+=(T const val)
    {
        *this += vecN(val);
        return *this;
    }
    vecN& operator-=(T const val)
    {
        *this -= vecN(val);
        return *this;
    }
    vecN& operator/=(T const val)
    {
        *this /= vecN(val);
        return *this;
    }

    // assign at
    T& operator[](uint32_t const idx) { return at(idx); }

    // get at
    T const& operator[](uint32_t const idx) const { return const_cast<vecN*>(this)->at(idx); }

    T& at(uint32_t idx)
    {
        if (idx >= N)
        {
            std::cout << "[vecN] Out of bounds access" << std::endl;
            idx = N;
        }
        return data_[idx];
    }

    // clang-format off
    template <
        typename Vec,
        std::enable_if_t<
            (std::is_same_v<Vec, vecN<typename Vec::value_type, Vec::height>>) &&
            (std::is_convertible_v<T, typename Vec::value_type>) &&
            (height == Vec::height),
            bool> = true
    >
    // clang-format on
    T dot(Vec const& other) const
    {
        // a.k.a. scalar product. sum(elemwise)
        return (*this * other).sum();
    }

    [[nodiscard]] T norm() const
    {
        T sum{static_cast<T>(0.0F)};
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            sum += std::pow(data_[idx], static_cast<T>(2.0F));
        }
        return std::sqrt(sum);
    }

    [[nodiscard]] T sum() const
    {
        T sum{static_cast<T>(0.0F)};
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            sum += data_[idx];
        }
        return sum;
    }

    [[nodiscard]] T minVal() const
    {
        T min{std::numeric_limits<T>::max()};
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            if (data_[idx] < min)
            {
                min = data_[idx];
            }
        }
        return min;
    }

    [[nodiscard]] T maxVal() const
    {
        T max{std::numeric_limits<T>::min()};
        for (uint32_t idx{0U}; idx < N; ++idx)
        {
            if (data_[idx] > max)
            {
                max = data_[idx];
            }
        }
        return max;
    }

 private:
    T data_[N]{};
};

template <typename T, uint32_t N>
std::ostream& operator<<(std::ostream& stream, vecN<T, N> const& arr)
{
    for (uint32_t idx{0U}; idx < N; ++idx)
    {
        stream << arr[idx] << ", ";
    }
    stream << " |";
    stream << std::endl;
    return stream;
}

}  // namespace core
}  // namespace kioku

#endif  // SRC_CORE_MATH_INCLUDE_MATH_VEC_N_H

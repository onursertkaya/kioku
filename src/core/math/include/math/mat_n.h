#ifndef SRC_CORE_MATH_INCLUDE_MATH_MAT_N_H
#define SRC_CORE_MATH_INCLUDE_MATH_MAT_N_H

#include <array>
#include <cmath>
#include <iomanip>

#include "math/util.h"
#include "math/vec_n.h"
#include "util/assert.h"

namespace kioku
{
namespace core
{

template <typename T, uint32_t N, uint32_t M>
class matN
{
 public:
    static constexpr uint32_t width{M};
    static constexpr uint32_t height{N};
    using value_type = T;

    explicit matN(T init_val)
    {
        for (uint32_t r{0U}; r < height; ++r)
        {
            data_[r] = vecN<T, M>(init_val);
        }
    }

    template <typename U = matN, std::enable_if_t<U::height == 1U, bool> = true>
    explicit matN(vecN<T, M> const& vec)
    {
        for (uint32_t row{0U}; row < N; ++row)
        {
            data_[row][0U] = vec[row];
        }
    }

    template <typename... Ts, uint32_t num_vecs = sizeof...(Ts)>
    explicit matN(Ts (&&... vecs)[M]) :
        data_{vecN<T, M>(vecs)...}
    {
        // kioku-TIL
        // cool stuff.
        // https://stackoverflow.com/questions/65305326/c-sfinae-double-nested-initializer-list-vs-variadic-template-constructor?rq=1
        static_assert(num_vecs == N);
    }

    // Copy ctor.
    matN(matN const& other)
    {
        for (uint32_t r{0U}; r < height; ++r)
        {
            data_[r] = other[r];
        }
    }

    // Move ctor.
    matN(matN&& other) noexcept { std::swap(data_, other.data_); }

    // Copy assignment.
    matN& operator=(matN const& other)
    {
        for (uint32_t r{0U}; r < height; ++r)
        {
            data_[r] = other[r];
        }
        return *this;
    }

    // Move assignment.
    matN& operator=(matN&& other) noexcept
    {
        std::swap(data_, other.data_);
        return *this;
    }

    // Dtor
    ~matN() = default;

    [[nodiscard]] vecN<T, M> const& row(uint32_t const row_idx) const
    {
        // static_assert(row_idx < N); //fixme: must be at runtime
        return data_[row_idx];
    }

    // wtf, how does this even work without const casting?!
    vecN<T, M> const& operator[](uint32_t const row_idx) const { return row(row_idx); }

    vecN<T, M>& operator[](uint32_t const row_idx) { return row(row_idx); }

    vecN<T, M>& row(uint32_t const row_idx)
    {
        // static_assert(row_idx < N); //fixme: must be at runtime
        return data_[row_idx];
    }

    // constructs a new vecN instance as we don't have the original
    // vecN for a given col in contagious memory. it's expensive anyway
    [[nodiscard]] vecN<T, N> col(uint32_t const col_idx) const
    {
        // static_assert(col_idx < M); //fixme: must be at runtime

        vecN<T, N> selected_col{};
        for (uint32_t row{0U}; row < N; ++row)
        {
            selected_col[row] = data_[row][col_idx];
        }
        return selected_col;
    }

    matN operator+(matN const& other) const
    {
        matN retval(0U);
        for (uint32_t r{0U}; r < height; ++r)
        {
            retval[r] = data_[r] + other[r];
        }
        return retval;
    }

    matN operator-(matN const& other) const
    {
        matN retval(0U);
        for (uint32_t r{0U}; r < height; ++r)
        {
            retval[r] = data_[r] - other[r];
        }
        return retval;
    }

    matN operator*(T val) const
    {
        matN retval(0U);
        for (uint32_t r{0U}; r < height; ++r)
        {
            retval[r] = data_[r] * val;
        }
        return retval;
    }

    [[nodiscard]] auto transpose() const
    {
        // fixme, lazily compute and cache. whenever a change occurs (with
        // operator[]), must be called again.
        matN<T, M, N> retval(static_cast<T>(0.0F));
        for (uint32_t i{0U}; i < M; ++i)
        {
            retval[i] = this->col(i);
        }
        return retval;
    }

    template <typename Mat,
              std::enable_if_t<
                  (std::is_same_v<Mat, matN<T, Mat::height, Mat::width>>)&&(Mat::height == width),
                  bool> = true>
    [[nodiscard]] auto dot(Mat const& other) const
    {
        // static_assert();
        matN<T, height, Mat::width> retval(static_cast<T>(0.0F));

        auto const other_T{other.transpose()};
        for (uint32_t i{0U}; i < retval.height; ++i)
        {
            auto const& row_vec{data_[i]};
            for (uint32_t j{0U}; j < retval.width; ++j)
            {
                retval[i][j] = (row_vec * other_T[j]).sum();
            }
        }
        return retval;
    }

    template <typename Vec,
              std::enable_if_t<(std::is_same_v<Vec, vecN<T, Vec::height>>)&&(Vec::height == width),
                               bool> = true>
    auto dot(Vec const& vec) const
    {
        matN<T, height, 1U> retval(static_cast<T>(0.0F));

        for (uint32_t i{0U}; i < height; ++i)
        {
            retval[i][0U] = (data_[i] * vec).sum();
        }
        return retval;
    }

    // so this works
    template <typename U = matN,
              std::enable_if_t<(U::height == U::width) && (U::height == 2U), bool> = true>

    // //but this does not
    // template <
    //     std::enable_if_t<
    //         ((height==width) && (height==2U)),
    //     bool> = true
    // >
    [[nodiscard]] auto inv() const
    {
        T const det{data_[0U][0U] * data_[1U][1U] - data_[0U][1U] * data_[1U][0U]};
        KIOKU_ASSERT(!math::isNear(std::abs(det), 0.0F));

        matN retval(static_cast<T>(0U));

        retval[0U][0U] = data_[1U][1U] / det;
        retval[0U][1U] = -data_[1U][0U] / det;
        retval[1U][0U] = -data_[0U][1U] / det;
        retval[1U][1U] = data_[0U][0U] / det;

        return retval;
    }

    // 3x3 fast, eigen decomp, cholesky,

 private:
    vecN<T, M> data_[N];
};

template <typename T, uint32_t N, uint32_t M>
std::ostream& operator<<(std::ostream& stream, matN<T, N, M> const& arr)
{
    for (uint32_t row{0U}; row < N; ++row)
    {
        for (uint32_t col{0U}; col < M; ++col)
        {
            stream << std::fixed << std::setw(9) << std::setprecision(4) << arr[row][col] << ", ";
        }
        stream << std::endl;
    }
    stream << std::endl;
    return stream;
}

}  // namespace core
}  // namespace kioku

#endif  // SRC_CORE_MATH_INCLUDE_MATH_MAT_N_H

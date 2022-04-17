#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_STATIC_ARRAY_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_STATIC_ARRAY_H

#include <algorithm>
#include <iostream>

#include "util/assert.h"

namespace kioku
{
namespace core
{

// // check effective modern c++ for type deduction rules, both for auto and decltype
// template <typename T>
// constexpr decltype(auto) toConst(T&& obj)
// {
//     return static_cast<decltype(obj) const>(obj);
// }

/// A fixed-size buffer with indexed access, a.k.a. an array-based List.
/// Does not throw exceptions.
/// All memory is allocated at initialization.
/// There'll be no memory management during object lifetime.
template <typename T, std::size_t N, int FillVal = 0>
class StaticArrayDeprecated
{
 public:
    using value_type = T;
    using index_type = decltype(N);

    static constexpr T default_value{static_cast<T>(FillVal)};

    // Copy ctor (1/5)
    StaticArrayDeprecated(StaticArrayDeprecated const& other, T const fill_val = T{}) :
        ctr_{other.ctr_}
    {
        std::copy(other.begin(), other.end(), this->begin());

        // fill only if a fill_val is requested.
        if (fill_val != T{})
        {
            fill_rest(fill_val);
        }
    }

    // Copy assignment (2/5)
    StaticArrayDeprecated& operator=(StaticArrayDeprecated const& other)
    {
        ctr_ = other.ctr_;
        std::copy(other.begin(), other.end(), this->begin());
        return *this;
    }

    // Variadic ctor.
    // Requires at least two instances of T in the parameter list. Aim is to prevent the semantic
    // clash of "is this the size parameter, or an element?" which e.g. std::vector has.
    template <typename... Ts, index_type NumTs = sizeof...(Ts),
              std::enable_if_t<(std::is_same_v<Ts, T> && ...) && (NumTs > 1U), bool> = true>
    explicit StaticArrayDeprecated(Ts&&... elems) :
        data_{elems...},
        ctr_{NumTs}
    {
    }

    // Ctor with fill value, a.k.a. initialization value.
    // Instance will still be empty.
    static StaticArrayDeprecated createFill(T const fill_val = T{})
    {
        StaticArrayDeprecated retval{};
        std::fill(std::begin(retval.data_), std::end(retval.data_), fill_val);
        return retval;
    }

    // Ctor for constucting from other instance with different buffer capacity,
    // optionally using a custom fill value.
    template <typename U = T, index_type M>
    static StaticArrayDeprecated createFrom(StaticArrayDeprecated<U, M> const& other,
                                            U const fill_val = U{})
    {
        static_assert(M < N);  // for M==N, use copy ctor instead.
        StaticArrayDeprecated retval{};
        retval.ctr_ = other.size();
        std::copy(other.begin(), other.end(), retval.begin());
        if (fill_val != U{})
        {
            retval.fill_rest(fill_val);
        }
        return retval;
    }

    // Constructor with any container type that defines
    // ::value_type within its declaration.
    // Can be an STL vector, array, list etc.
    template <typename Container,
              std::enable_if_t<std::is_same_v<typename Container::value_type, T>, bool> = true>
    explicit StaticArrayDeprecated(Container array, T fill_val = T{}) :
        ctr_{array.size()}
    {
        KIOKU_ASSERT(array.size() > 0);
        KIOKU_ASSERT(array.size() <= N);

        std::copy(array.begin(), array.end(), this->begin());
        fill_rest(fill_val);
    }

    index_type size() const noexcept { return ctr_; }
    [[nodiscard]] bool isEmpty() const noexcept { return size() == 0U; }
    constexpr index_type bufferCapacity() const noexcept { return N; }

    T& operator[](index_type const idx) noexcept { return at(idx); }

    T const& operator[](index_type const idx) const noexcept
    {
        return const_cast<StaticArrayDeprecated*>(this)->at(idx);
    }

    T& at(index_type idx) noexcept
    {
        // todo, handle idx > ctr_-1, that also should be an invalid access - but maybe only when
        // reading? i.e. const version of operator[].
        if (idx >= N)
        {
            std::cout << "[StaticArrayDeprecated] operator[](): Out of bounds "
                      << "access at idx: " << idx << std::endl;
            idx = N - 1U;
        }
        return data_[idx];
    }

    void push_back(T const elem) noexcept
    {
        if (ctr_ < N)
        {
            data_[ctr_] = elem;
            ++ctr_;
        }
        else
        {
            std::cout << "[StaticArrayDeprecated] push_back(): Capacity full, "
                      << "not inserting element: " << elem << std::endl;
        }
    }

    void pop_back() noexcept
    {
        if (ctr_ > 0U)
        {
            data_[ctr_] = default_value;
            --ctr_;
        }
        else
        {
            std::cout << "[StaticArrayDeprecated] pop_back(): Array empty." << std::endl;
        }
    }

    T* begin() { return static_cast<T*>(data_); }

    // clang is OK, g++ is not
    T const* begin() const { return static_cast<T const*>(data_); }

    T* end() { return begin() + static_cast<std::ptrdiff_t>(ctr_); }

    // clang is OK, g++ is not
    T const* end() const { return begin() + static_cast<std::ptrdiff_t>(ctr_); }

 private:
    T data_[N]{};
    index_type ctr_{};

    StaticArrayDeprecated() = default;

    void fill_rest(T const val) { std::fill(this->end(), std::end(data_), val); }
};

template <typename T, std::size_t N>
std::ostream& operator<<(std::ostream& stream, StaticArrayDeprecated<T, N> const& arr)
{
    for (auto const& e : arr)
    {
        stream << e << ",";
    }
    stream << std::endl;
    return stream;
}

}  // namespace core
}  // namespace kioku

#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_STATIC_ARRAY_H

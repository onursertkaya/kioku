#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_BUFFER_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_BUFFER_H
#include <cstddef>

#include "util/assert.h"

namespace kioku
{
namespace core
{

// Generic contiguous data buffer with fixed size.
template <typename T, std::size_t N>
class StaticBuffer
{
    static_assert(N > 0U, "StaticBuffer must have a positive size.");
    static_assert(!std::is_const_v<T>, "StaticBuffer should have a non-const type. Constness must "
                                       "be achieved by marking the variable const.");

 public:
    using value_type = T;

    StaticBuffer() = default;
    ~StaticBuffer() = default;

    StaticBuffer(StaticBuffer&) = delete;
    StaticBuffer(StaticBuffer&&) = delete;
    StaticBuffer& operator=(StaticBuffer&) = delete;
    StaticBuffer& operator=(StaticBuffer&&) = delete;

    // Variadic ctor.
    template <typename... Ts, std::size_t NumTs = sizeof...(Ts),
              std::enable_if_t<(std::is_same_v<Ts, T> && ...) && (NumTs > 1U), bool> = true>
    explicit StaticBuffer(Ts&&... elems) :
        data_{elems...}
    {
    }

    T* data() { return data_; }
    T const* data() const { return data_; }

    T& operator[](std::size_t const idx) { return data_[idx]; }
    T operator[](std::size_t const idx) const { return data_[idx]; }

    [[nodiscard]] std::size_t size() const { return size_; }

 private:
    static constexpr std::size_t size_{N};
    T data_[N]{};
};

// Generic contiguous data buffer with adjustable size.
template <typename T>
class DynamicBuffer
{
    static_assert(!std::is_const_v<T>, "DynamicBuffer should have a non-const type. Constness must "
                                       "be achieved by marking the variable const.");

 public:
    using value_type = T;

    explicit DynamicBuffer(std::size_t const size) :
        size_{size}
    {
        // TODO:
        // It does not make sense to create a const adjustable buffer through this ctor.
        // find a way to detect and prevent at compile time, either with a working version of the
        // following or a factory function.
        // static_assert(!std::is_const_v<decltype(*this)>, "");
        KIOKU_ASSERT(size_ > 0U);
        ptr_ = new T[size_]{};
    }

    // Construct from a container.
    template <typename Container,
              std::enable_if_t<std::is_same_v<typename Container::value_type, T>, bool> = true>
    explicit DynamicBuffer(Container const& array) :
        size_{array.size()}
    {
        KIOKU_ASSERT(size_ > 0);
        ptr_ = new T[size_]{};
        std::copy(array.begin(), array.end(), begin());
    }

    // Copy/move is not allowed.
    DynamicBuffer(DynamicBuffer&) = delete;
    DynamicBuffer(DynamicBuffer&&) = delete;
    DynamicBuffer& operator=(DynamicBuffer&) = delete;
    DynamicBuffer& operator=(DynamicBuffer&&) = delete;

    ~DynamicBuffer() { delete[] ptr_; }

    T* data() { return ptr_; }
    T const* data() const { return ptr_; }

    T& operator[](std::size_t const idx) { return ptr_[idx]; }
    T operator[](std::size_t const idx) const { return ptr_[idx]; }

    [[nodiscard]] std::size_t size() const { return size_; }

    void reallocate(std::size_t const new_size)
    {
        KIOKU_ASSERT(new_size > size_);

        T* temporary{new T[new_size]{}};
        std::copy(begin(), end(), temporary);
        reset();

        size_ = new_size;
        ptr_ = temporary;
    }

 private:
    std::size_t size_;
    T* ptr_{nullptr};

    void reset()
    {
        delete[] ptr_;
        ptr_ = nullptr;
        size_ = 0U;
    }

    T* begin() { return ptr_; }

    T* end() { return ptr_ + static_cast<std::ptrdiff_t>(size_); }
};

}  // namespace core
}  // namespace kioku
#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_BUFFER_H

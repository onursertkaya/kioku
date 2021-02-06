#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_BUFFER_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_BUFFER_H
#include <cstddef>

#include "util/assert.h"

namespace kioku
{
namespace core
{

// Generic contiguous data buffer with adjustable size.
template <typename T>
class DynamicBuffer
{
 public:
    explicit DynamicBuffer<T>(std::size_t const size) : size_{size}
    {
        KIOKU_ASSERT(size > 0U);
        ptr_ = new T[size_]{};
    }

    // Copy/move is not allowed.
    DynamicBuffer(DynamicBuffer&) = delete;
    DynamicBuffer(DynamicBuffer&&) = delete;
    DynamicBuffer& operator=(DynamicBuffer&) = delete;
    DynamicBuffer& operator=(DynamicBuffer&&) = delete;

    ~DynamicBuffer() { delete[] ptr_; }

    T& operator[](std::size_t const idx) { return ptr_[idx]; }

    [[nodiscard]] std::size_t size() const { return size_; }

    void reset()
    {
        delete[] ptr_;
        ptr_ = nullptr;
        size_ = 0U;
    }

    T* begin() { return ptr_; }

    T* end() { return ptr_ + static_cast<std::ptrdiff_t>(size_); }

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
};

// Generic contiguous data buffer with fixed size.
template <typename T, std::size_t N>
class StaticBuffer
{
    static_assert(N > 0U, "Static buffer must have a positive size.");

 public:
    StaticBuffer() = default;
    ~StaticBuffer() = default;

    // copy ctor
    StaticBuffer(StaticBuffer const& other)
    {
        std::copy(other.data_, other.data_ + std::ptrdiff_t{N}, data_);
    }

    // copy assignment
    StaticBuffer& operator=(StaticBuffer const& other)
    {
        if (this != &other)
        {
            std::copy(other.data_, other.data_ + std::ptrdiff_t{N}, data_);
        }
        return *this;
    }

    // no move allowed
    StaticBuffer& operator=(StaticBuffer&&) = delete;
    StaticBuffer(StaticBuffer&&) = delete;

    T& operator[](std::size_t const idx) { return data_[idx]; }

    [[nodiscard]] std::size_t size() const { return size_; }

 private:
    static constexpr std::size_t size_{N};
    T data_[N]{};
};

}  // namespace core
}  // namespace kioku
#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_BUFFER_H

#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_VIEW_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_VIEW_H
#include <cstddef>

#include "util/assert.h"

namespace kioku
{
namespace core
{

// Generic view class for any kind of iterable container.
template <typename T>
class View
{
 public:
    using value_type = std::remove_cv_t<T>;
    using const_value_type = value_type const;

    template <typename Container, typename U = typename Container::value_type>
    explicit View(Container& array) :
        data_{array.data()},
        size_{array.size()}
    {
        static_assert(std::is_same_v<U, value_type>, "Incompatible underlying type.");
    }

    // create a new view, pointing to the same array, with narrower range.
    View range(std::size_t const start, std::size_t const end) const
    {
        KIOKU_ASSERT(end > start);
        KIOKU_ASSERT(end < size_);
        return View(data_, start, end);
    }

    [[nodiscard]] std::size_t size() const { return size_; }

    T& at(std::size_t const idx) const { return data_[idx]; }

    T* begin() const { return data_; }
    T* end() const { return data_ + static_cast<ptrdiff_t>(size_); }

 private:
    T* data_;
    std::size_t size_;

    explicit View(T* data, std::size_t const start, std::size_t const end) :
        data_{data + static_cast<std::ptrdiff_t>(start)},
        size_{end - start}
    {
    }
};

// Provide vector-like interface for push/pop, on top of View.
template <typename T>
class DynamicView
{
 public:
    using value_type = std::remove_cv_t<T>;
    using const_value_type = value_type const;
    static_assert(!std::is_const_v<T>, "Const type is not allowed in DynamicView.");

    template <typename Container, typename U = typename Container::value_type>
    explicit DynamicView(Container& array) :
        view_{View<value_type>{array}}
    {
    }

    void push_back(T const& elem) const noexcept
    {
        if (ctr_ < view_.size())
        {
            view_.at(ctr_) = elem;
            ++ctr_;
        }
        else
        {
            std::cout << "[DynamicView] "
                      << "push_back(): Capacity full at idx: " << ctr_ << ", "
                      << "not inserting element: " << elem << std::endl;
        }
    }

    void pop_back() const noexcept
    {
        if (ctr_ > 0U)
        {
            view_.at(ctr_) = T{};
            --ctr_;
        }
        else
        {
            std::cout << "[DynamicView] "
                      << "pop_back(): Array empty." << std::endl;
        }
    }

    [[nodiscard]] std::size_t size() const { return ctr_; }

    T& at(std::size_t const idx) const { return view_.at(idx); }

    T* begin() const { return view_.begin(); }
    T* end() const { return view_.end(); }

 private:
    View<value_type> view_;
    std::size_t mutable ctr_{0U};
};

template <typename Container>
decltype(auto) createView(Container& array)
{
    return View<typename Container::value_type>(array);
}

template <typename Container>
decltype(auto) createConstView(Container const& array)
{
    return View<typename Container::value_type const>(array);
}

template <typename Container>
decltype(auto) createDynamicView(Container& array)
{
    return DynamicView<typename Container::value_type>(array);
}

}  // namespace core
}  // namespace kioku
#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_VIEW_H

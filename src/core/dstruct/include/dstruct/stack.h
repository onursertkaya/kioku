#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_STACK_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_STACK_H
#include "dstruct/buffer.h"

namespace kioku
{
namespace core
{
// LIFO queue with runtime-adjustable capacity.
template <typename T>
class Stack
{
 public:
    static constexpr std::size_t initial_capacity{2U};

    void push(T const elem)
    {
        data_[idx_] = elem;
        ++idx_;
    }

    T pop()
    {
        if (size() == 0U)
        {
            std::cout << "Empty stack." << std::endl;
            return T{};
        }
        --idx_;
        return data_[idx_];
    }

    std::size_t size() { return idx_; }

    bool empty() { return size() == 0U; }

 private:
    DynamicBuffer<T> data_{initial_capacity};
    std::size_t idx_{0U};

    // todo, expect an allocator object as a template param instead.
    void calibrateBuffer()
    {
        if (idx_ + 1U == data_.size())
        {
            data_.reallocate(2U * data_.size());
        }
    }
};

}  // namespace core
}  // namespace kioku
#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_STACK_H

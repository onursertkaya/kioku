#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_QUEUE_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_QUEUE_H
#include "dstruct/buffer.h"

namespace kioku
{
namespace core
{

// FIFO queue with runtime-adjustable capacity.
template <typename T>
class Queue
{
 public:
    static constexpr std::size_t initial_capacity{2U};

    void push(T const elem)
    {
        data_[back_idx_] = elem;
        ++back_idx_;
    }

    T pop()
    {
        if (size() == 0U)
        {
            std::cout << "Empty queue." << std::endl;
            return T{};
        }
        auto& retval = data_[front_idx_];
        ++front_idx_;
        return retval;
    }

    std::size_t size() { return back_idx_ - front_idx_; }

    bool empty() { return size() == 0U; }

 private:
    DynamicBuffer<T> data_{initial_capacity};
    std::size_t front_idx_{0U};
    std::size_t back_idx_{0U};

    // todo, expect an allocator object as a template param instead.
    void calibrateBuffer()
    {
        if (size() == data_.size())
        {
            data_.reallocate(2U * data_.size());
        }
    }
};

}  // namespace core
}  // namespace kioku
#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_QUEUE_H

#ifndef SRC_CORE_ALGO_INCLUDE_ALGO_SORT_H
#define SRC_CORE_ALGO_INCLUDE_ALGO_SORT_H

#include "algo/search.h"
#include "dstruct/static_array.h"
#include "dstruct/view.h"

#include <cstdint>
#include <iterator>
#include <limits>

#include <iostream>
namespace kioku
{
namespace algo
{
namespace sort
{

template <typename Container, typename T = typename Container::value_type>
inline void selection(Container& array)
{
    // kioku-TIL
    // summary:
    //     Starting from [0]th element in a zero indexed array.
    //     finds the minimum value in the *right* sub-array and
    //     swaps it with the current element.
    // pros:
    //     - No extra memory required, in-place sorting.
    // cons:
    //     - O(n^2): (n + (n-1) + ... + 1) -> (n x (n + 1))/2
    //     - Temporary variables needed.
    // misc:
    //     - Looks forward to find mins.
    //     - Should be used when swap and T is memory-heavy.

    for (auto i = array.begin(); i < array.end(); ++i)
    {
        T min_val{std::numeric_limits<T>::max()};
        uint32_t min_val_idx{0U};
        auto j = i;
        while (j < array.end())
        {
            if (*j < min_val)
            {
                min_val = *j;
                min_val_idx = std::distance(i, j);
            }
            ++j;
        }
        std::iter_swap(i, std::next(i, min_val_idx));
    }
}

template <typename Container, typename T = typename Container::value_type>
inline void insertion(Container& array)
{
    // kioku-TIL
    // summary:
    //     Starting from [1]st element in a zero indexed array,
    //     swaps each element with *previous ones* until the left
    //     sub-array is sorted.
    // pros:
    //     - No extra memory required, in-place sorting.
    //     - No temporaries needed, only comparison and swap.
    // cons:
    //     - O(n^2): O(n^2) comparisons + O(n^2) swaps
    // comments:
    //     - Looks backward to find mins.
    //     - Should be used when swap and T is memory-cheap.

    for (auto i = std::next(array.begin()); i < array.end(); ++i)
    {
        auto j = i;
        while (*j < *std::prev(j) and j != array.begin())
        {
            std::iter_swap(j, std::prev(j));
            --j;
        }
    }
}

template <typename Container, typename T = typename Container::value_type>
inline void bubble(Container& array)
{
    // kioku-TIL
    // summary:
    //     Starting from [0]th element in a zero indexed array.
    //     swaps current and next elements until no swap occurs.
    // pros:
    //     - No extra memory required, in-place sorting.
    //     - No temporaries needed.
    // cons:
    //     - O(n^2): O(n^2) comparisons + O(n^2) swaps
    // misc:
    //    - Looks neighborhood to find mins.
    //    - Should be used when swap and T is memory-cheap.

    bool swap_occurred{true};
    while (swap_occurred)
    {
        swap_occurred = false;
        for (auto i = array.begin(); i < array.end() - std::ptrdiff_t{1U}; ++i)
        {
            auto j = std::next(i);
            if (*i > *j)
            {
                std::iter_swap(i, j);
                swap_occurred = true;
            }
        }
    }
}

template <typename Container, typename T = typename Container::value_type>
inline void merge(Container& array)
{
    // kioku-TIL
    // summary:
    //     This is the bottom-up version: No recursive sh*t.
    //     -> Group the elements with increasing sub-arrays of size
    //        2, 4, 8...
    //     -> In each group, divide the group into left and right.
    //     -> Pick the smallest element at the heads of left and right,
    //        until both are exhausted.
    //     -> Continue until group size reaches the array size.
    // pros:
    //     - O(n log(n)):
    //     - Efficient, as stride grows quickly with powers of 2.
    // cons:
    //     - Needs scratch memory to work on, does not work with swaps.
    // notes:
    //     - It took A LOT of time to keep it right. The idea is simple
    //       and fun, however all the small details with out-of-bounds
    //       checking, edge cases, array size etc. makes the
    //       implementation challenging.

    constexpr auto maxval = std::numeric_limits<T>::max();
    auto out = array;
    uint32_t out_ctr{0U};

    for (uint32_t stride{1U}; stride < out.size(); stride *= 2U)
    {
        auto const block_size{stride * 2U};
        auto const block_count{1U + (out.size() - 1U) / block_size};

        for (uint32_t block_idx{0U}; block_idx < block_count; ++block_idx)
        {
            auto const curr_block_head = block_idx * block_size;

            auto left_head = curr_block_head;
            auto right_head = left_head + stride;

            while (true)
            {
                bool const left_exhausted{(left_head == curr_block_head + stride) or
                                          left_head == array.size()};
                bool const right_exhausted{(right_head == curr_block_head + block_size) or
                                           right_head >= array.size()};

                if (left_exhausted and right_exhausted)
                {
                    break;
                }

                auto const left = left_exhausted ? maxval : array[left_head];
                auto const right = right_exhausted ? maxval : array[right_head];

                if (left <= right)
                {
                    out[out_ctr] = left;
                    ++out_ctr;
                    ++left_head;
                }
                else
                {
                    out[out_ctr] = right;
                    ++out_ctr;
                    ++right_head;
                }
            }
        }
        array = out;
        out_ctr = 0U;
    }
}

// TODO(unknown): implement the following:

// recursive version
// template <typename Container, typename T = typename Container::value_type>
// inline void mergeV2(Container& array)
// {

// }

// template <typename Container, typename T = typename Container::value_type>
// inline void heap(Container& array)
// {

// }

// template <typename Container, typename T = typename Container::value_type>
// inline void quick(Container& array)
// {

// }

}  // namespace sort
}  // namespace algo
}  // namespace kioku
#endif  // SRC_CORE_ALGO_INCLUDE_ALGO_SORT_H

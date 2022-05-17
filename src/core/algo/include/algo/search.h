#ifndef SRC_CORE_ALGO_INCLUDE_ALGO_SEARCH_H
#define SRC_CORE_ALGO_INCLUDE_ALGO_SEARCH_H
#include <cstdint>
#include <limits>

#include "dstruct/view.h"

namespace kioku
{
namespace algo
{
namespace search
{

enum class QueryMode : std::size_t
{
    kFirst = 0U,
    kLast = 1U,
    kAll = 2U  // todo, implement rest for this mode
};

template <typename T>
struct QueryResult
{
    std::size_t idx{-1U};
    T val{};
};

template <typename Container, typename T = typename Container::value_type>
inline QueryResult<T> linear(Container const& array, T const& value)
{
    auto c_view = createConstView(array);
    std::size_t idx{0U};
    for (auto const& elem : c_view)
    {
        if (elem == value)
        {
            break;
        }
        ++idx;
    }
    return {idx, value};
}

template <typename Container, typename T = typename Container::value_type>
inline QueryResult<T> binary(Container const& array, T const& value)
{
    // source:
    // Errichto | Binary Search Tutorial (C++ and Python) | watch?v=GU7DpgHINWQ

    // complexity: log2(N) as search space is halved in each iteration.
    // precondition: container c must be sorted.

    auto c_view = createConstView(array);
    std::size_t idx{0U};

    std::size_t left{0U};
    std::size_t right{c_view.size() - 1U};
    while (left <= right)
    {
        auto mid = left + ((right - left) / 2U);
        if (c_view.at(mid) < value)
        {
            left = mid + 1U;
        }
        else if (c_view.at(mid) > value)
        {
            right = mid - 1U;
        }
        else
        {
            idx = mid;
            break;
        }
    }

    return {idx, value};
}

template <typename Container, typename T = typename Container::value_type>
inline QueryResult<T> min(Container const& array)
{
    auto c_view = createConstView(array);

    std::size_t idx{0U};
    std::size_t min_val_idx{0U};
    T min_val{std::numeric_limits<T>::max()};
    for (auto const& elem : c_view)
    {
        if (elem < min_val)
        {
            min_val = elem;
            min_val_idx = idx;
        }
        idx++;
    }
    return {min_val_idx, min_val};
}

template <typename Container, typename T = typename Container::value_type>
inline QueryResult<T> max(Container const& array)
{
    auto c_view = createConstView(array);

    std::size_t idx{0U};
    std::size_t max_val_idx{0U};
    T max_val{std::numeric_limits<T>::min()};
    for (auto const& elem : c_view)
    {
        if (elem > max_val)
        {
            max_val = elem;
            max_val_idx = idx;
        }
        idx++;
    }
    return {max_val_idx, max_val};
}

/*
// TODO, implement the following:
template <typename Container, typename T = typename Container::value_type>
inline QueryResult<T> bfs(Container const& c)
{
    // sfinae out non-tree structs
}

template <typename Container, typename T = typename Container::value_type>
inline QueryResult<T> dfs(Container const& c)
{
    // sfinae out non-tree structs
}
*/

}  // namespace search
}  // namespace algo
}  // namespace kioku
#endif  // SRC_CORE_ALGO_INCLUDE_ALGO_SEARCH_H

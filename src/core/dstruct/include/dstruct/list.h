#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_LIST_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_LIST_H

#include <iostream>
#include <memory>
#include <utility>

#include "dstruct/node.h"

namespace kioku
{
namespace core
{

/// A singly-linked list with "smart" pointers.
/// By no means this is efficient code, nor it
/// is good practice. I just wanted to see how
/// it would look like with shared_ptr and
/// weak_ptr. Well, it looks bad, but at least
/// it works.
template <typename T>
class List
{
 public:
    List(std::initializer_list<T> const elems) :
        head_{std::make_shared<Node<T>>(*elems.begin(), nullptr)},
        size_{1U}
    {
        auto first_after_head = std::next(elems.begin());
        for (auto e_it = first_after_head; e_it != elems.end(); ++e_it)
        {
            this->append(*e_it);
        }
    }

    uint32_t size() { return size_; }

    T const& operator[](uint32_t const idx) const
    {
        auto node = nodeAt(idx);
        return node.lock()->elem;
    }

    T operator[](uint32_t const idx) { return const_cast<List const&>(*this)[idx]; }

    void append(T const elem)
    {
        auto node = nodeAt(size_ - 1U);
        node.lock()->next = std::make_shared<Node<T>>(elem, nullptr);
        ++size_;
    }

    void insert(T const elem, int32_t const idx)
    {
        constexpr int32_t kZero{0U};

        if (idx >= static_cast<int32_t>(size_))
        {
            append(elem);
        }
        else if (idx == kZero)
        {
            std::weak_ptr<Node<T>> old_head = head_;
            head_ = std::make_shared<Node<T>>(elem, old_head.lock());
            ++size_;
        }
        else
        {
            // if a negative idx was given, increment the
            // effective idx until it's positive.
            // no-op if the idx was already positive.
            int32_t effective_idx{idx};
            while (effective_idx < kZero)
            {
                effective_idx += size_;
            }

            auto node_prev = nodeAt(effective_idx - 1);
            auto insertion_node = node_prev.lock()->next;
            auto node_next = insertion_node->next;

            insertion_node = std::make_shared<Node<T>>(elem, insertion_node);
            node_prev.lock()->next = insertion_node;

            ++size_;
        }
    }

    bool remove(int32_t const idx)
    {
        // todo, implement
        static_cast<void>(idx);
        return false;
    }

 private:
    template <typename U = T>
    struct Node
    {
        Node(T e, std::shared_ptr<Node<U>> n) :
            elem{e},
            next{std::move(n)}
        {
        }
        U elem;
        std::shared_ptr<Node<U>> next;
    };

    std::weak_ptr<Node<T>> nodeAt(uint32_t const idx) const
    {
        std::weak_ptr<Node<T>> ref{head_};

        uint32_t ctr{0U};
        while (ctr < idx)
        {
            ref = ref.lock()->next;
            ++ctr;
        }
        return ref;
    }

    std::shared_ptr<Node<T>> head_;
    uint32_t size_{0U};
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, List<T>& list)
{
    for (uint32_t i{0U}; i < list.size(); ++i)
    {
        stream << list[i] << ", ";
    }
    stream << std::endl;
    return stream;
}

}  // namespace core
}  // namespace kioku

#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_LIST_H

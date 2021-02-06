#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_NODE_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_NODE_H
#include <cstdint>

namespace kioku
{
namespace core
{

enum class NodeType : std::uint8_t
{
    kSingleLinkedList = 0U,
    kDoubleLinkedList = 1U,
    kBinaryTree = 2U,
};

template <typename T, NodeType type>
struct Node;

template <typename T>
struct Node<T, NodeType::kSingleLinkedList>
{
    explicit Node(T const e) : elem_{e} {}
    T elem_{};
    Node* next_{nullptr};
};

template <typename T>
struct Node<T, NodeType::kDoubleLinkedList>
{
    explicit Node(T const e) : elem_{e} {}
    T elem_{};
    Node* next_{nullptr};
    Node* prev_{nullptr};
};

template <typename T>
struct Node<T, NodeType::kBinaryTree>
{
    explicit Node(T const e) : elem_{e} {}
    T elem_{};
    Node* parent_{nullptr};
    Node* left_{nullptr};
    Node* right_{nullptr};
};

// =====

enum class Traversal : std::uint8_t
{
    kBreadthFirst = 0U,
    kPreOrder = 1U,
    kInOrder = 2U,
    kPostOrder = 3U,
};

}  // namespace core
}  // namespace kioku

// todo, with using declarations, define
// a tree node:  .left .right
// a graph node:
#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_NODE_H

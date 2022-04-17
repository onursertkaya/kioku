#ifndef SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_TREE_H
#define SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_TREE_H

#include <iostream>
#include <memory>
#include <utility>

#include "dstruct/node.h"
#include "dstruct/stack.h"

namespace kioku
{
namespace core
{
// https://www.youtube.com/playlist?list=PL4IH6CVPpTZUBQrSk5ZNTFdCphafDrsJA

// "smart data structures and dumb code are a lot better than the other way around."

// Acyclic graph with at most two child nodes at a parent node.
// Degree (connectivity) of a node is at most 3: 1 parent node + 2 child nodes
// Node namings: Root, internal, leaves.
// Can be balanced/unbalanced. In the worst case, it becomes a linked-list.
// Depth of a node: Number of edges on the path from root to the node.
// Tree depth d: Largest depth between root and a leave.
// Number of nodes in a fully-grown tree:
// N = 1 + 2 + 4 + ... + 2^d = 2^(d+1) - 1

// Traversal Complexity:
// Each node is visited and/or processed up to 3 times, depending on the traversal strategy -> O(N)
// Only keep the current node, the previous node + stack for local memory. efficient.
// Recursive calls to traversal functions are seemingly simpler, but well, recursion.

// Rationale:
// A `BinaryTree` by itself has no advantage as a data structure. Rather it becomes
// advantageous when certain constraints are introduced on the way of key storage.
//
// For a given parent P, left child C_L and right child C_R, if
// - P > C_L and P > C_R: max-heap
// - P < C_L and P < C_R: min-heap
// - P > C_L and P < C_R: binary search tree
//
// For example, in the case of binary search tree, as it's guaranteed to have the left
// child smaller and the right child larger, number of remaining elements to compare is cut
// in half with each consecutive level of depth. This means that the search functionality
// is O(log(N)) = O(log(2^d)) = O(d)
//
// This is the case only when the tree is fully balanced though. If it's skewed, say in the
// worst case of being a linked-list, then it becomes O(N) as there's no left child.
//
// Solution is the "self-balancing-binary trees". Upon insertion/deletion, they reorganize
// the nodes such that the tree is balanced as much as possible.
// AVL Trees, B-trees, Red-Black Trees, Splay Trees, Treaps
template <typename T>
class BinaryTree
{
 public:
    using TreeNode = Node<T, NodeType::kBinaryTree>;
    using value_type = T;

    explicit BinaryTree(T const root_key) :
        root_{new TreeNode{root_key}}
    {
    }

    void setLeft(TreeNode* left) { root_->left_ = left; }

    void setRight(TreeNode* right) { root_->right_ = right; }

    void traverse(Traversal const policy)
    {
        switch (policy)
        {
        case Traversal::kPreOrder: {
            preOrder();
            break;
        }
        case Traversal::kInOrder: {
            inOrder();
            break;
        }
        case Traversal::kPostOrder: {
            KIOKU_ASSERT(false);
            break;
        }
        case Traversal::kBreadthFirst: {
            KIOKU_ASSERT(false);
            break;
        }
        }
    }

 protected:
    TreeNode* root_{nullptr};

    void preOrder()
    {
        // INPUT: Binary Tree, T
        // OUTPUT: A preorder traversal of T
        // S <- empty_stack
        // push T.root to S
        // while S is not empty
        // u <- S.pop
        // process u
        // push u.right to S
        // push u.left to S

        Stack<TreeNode*> stack{};
        stack.push(root_);
        while (!stack.empty())
        {
            TreeNode* current_node{stack.pop()};

            std::cout << current_node->elem_ << std::endl;  // process

            if (current_node->right_ != nullptr)
            {
                stack.push(current_node->right_);
            }
            if (current_node->left_ != nullptr)
            {
                stack.push(current_node->left_);
            }
        }
    }

    void inOrder()
    {
        // INPUT: Binary Tree, T
        // OUTPUT: A preorder traversal of T
        // S <- empty_stack
        // u <- T.root
        // while S is not empty OR u != null
        // if u != null
        // push u to S
        // u <- u.left
        // else
        // u <- S.pop
        // process u
        // u <- u.right
        Stack<TreeNode*> stack{};
        TreeNode* current_node{root_};
        while (!stack.empty() || current_node != nullptr)
        {
            if (current_node != nullptr)
            {
                // while descending, seeing this for the first time
                stack.push(current_node);
                current_node = current_node->left_;
            }
            else
            {
                // while ascending back to top, seeing this node for the second time
                current_node = stack.pop();

                std::cout << current_node->elem_ << std::endl;  // process

                current_node = current_node->right_;
            }
        }
    }

    void postOrder() {}

    void breadthFirst() {}
};

template <typename T>
class BinarySearchTree : public BinaryTree<T>
{
 public:
    using TreeNode = typename BinaryTree<T>::TreeNode;

    explicit BinarySearchTree(T const root_key) :
        BinaryTree<T>{root_key}
    {
    }

    T search(T const key)
    {
        T retval{};
        TreeNode* node = getNode(key);
        if (node == nullptr)
        {
            std::cout << key << " not found." << std::endl;
        }
        else
        {
            retval = node->elem_;
        }
        return retval;
    }

    void getSorted() { BinaryTree<T>::inOrder(); }

 private:
    TreeNode* getNode(T const key)
    {
        TreeNode* current_node{this->root_};
        while (current_node != nullptr && current_node->elem_ != key)
        {
            if (key <= current_node->elem_)
            {
                current_node = current_node->left_;
            }
            else
            {
                current_node = current_node->right_;
            }
        }
        return current_node;
    }
};

// enum class HeapType : std::uint8_t
// {
//     kMin = 0,
//     kMax = 1,
// };

// template <typename T, HeapType type>
// class Heap : public BinaryTree<T>
// {
//  public:
//     static constexpr HeapType heap_type{type};
// };

}  // namespace core
}  // namespace kioku

#endif  // SRC_CORE_DSTRUCT_INCLUDE_DSTRUCT_TREE_H

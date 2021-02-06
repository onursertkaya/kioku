#include "dstruct/tree.h"

#include "gtest/gtest.h"

namespace kioku
{
namespace core
{

TEST(BinaryTree, ctor)
{
    using CharTree = BinaryTree<char>;
    using CTNode = CharTree::TreeNode;

    auto a = CharTree{'a'};

    auto f = CTNode{'f'};
    auto e = CTNode{'e'};
    auto d = CTNode{'d'};
    auto c = CTNode{'c'};
    auto b = CTNode{'b'};

    b.left_ = &d;
    b.right_ = &e;

    c.right_ = &f;

    a.setLeft(&b);
    a.setRight(&c);

    // a------b----d
    // \      \----e
    // \--c
    //    \--f

    testing::internal::CaptureStdout();
    a.traverse(Traversal::kPreOrder);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_STREQ(output.c_str(), "a\nb\nd\ne\nc\nf\n");

    testing::internal::CaptureStdout();
    a.traverse(Traversal::kInOrder);
    output = testing::internal::GetCapturedStdout();

    ASSERT_STREQ(output.c_str(), "d\nb\ne\na\nc\nf\n");
}

TEST(BinarySearchTree, ctor)
{
    using IntBST = BinarySearchTree<int>;
    using IntNode = IntBST::TreeNode;

    auto bst = IntBST{50};

    auto b = IntNode{40};
    auto c = IntNode{20};
    auto d = IntNode{45};
    auto e = IntNode{10};
    auto f = IntNode{24};
    auto g = IntNode{49};
    auto h = IntNode{2};
    auto i = IntNode{15};
    auto j = IntNode{28};
    auto k = IntNode{48};
    auto l = IntNode{12};
    auto m = IntNode{60};
    auto n = IntNode{90};
    auto o = IntNode{80};
    auto p = IntNode{95};
    auto q = IntNode{75};
    auto r = IntNode{85};

    bst.setLeft(&b);
    bst.setRight(&m);

    b.left_ = &c;
    b.right_ = &d;

    c.left_ = &e;
    c.right_ = &f;

    e.left_ = &h;
    e.right_ = &i;

    i.left_ = &l;

    f.right_ = &j;

    d.right_ = &g;

    g.left_ = &k;

    m.right_ = &n;

    n.left_ = &o;
    n.right_ = &p;

    o.left_ = &q;
    o.right_ = &r;

    // since it exists, returned val should be equal to searched val
    auto val = bst.search(90);
    ASSERT_EQ(val, 90);

    // since it does not exist, it should be the default T{}
    testing::internal::CaptureStdout();
    val = bst.search(128);
    std::string const output = testing::internal::GetCapturedStdout();

    ASSERT_STREQ(output.c_str(), "128 not found.\n");
    ASSERT_EQ(val, int{});

    // todo, crashes:
    // testing::internal::CaptureStdout();
    // bst.getSorted();
    // std::string output = testing::internal::GetCapturedStdout();

    // ASSERT_STREQ(output.c_str(), "2\n10\n12\n15\n20\n24\n28\n");
}

}  // namespace core
}  // namespace kioku

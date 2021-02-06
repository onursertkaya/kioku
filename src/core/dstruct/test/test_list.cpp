#include "dstruct/list.h"

#include "gtest/gtest.h"

namespace kioku
{
namespace core
{

TEST(TestList, single_init_and_access)
{
    List<float> a{1.2F};

    ASSERT_FLOAT_EQ(a[0U], 1.2F);
}

TEST(TestList, multi_init_and_access)
{
    List<float> a{1.0F, 2.0F, 3.0F};

    ASSERT_FLOAT_EQ(a[0U], 1.0F);
    ASSERT_FLOAT_EQ(a[1U], 2.0F);
    ASSERT_FLOAT_EQ(a[2U], 3.0F);
}

TEST(TestList, append)
{
    List<float> a{1.1F, 2.2F};

    a.append(3.3F);

    ASSERT_FLOAT_EQ(a[0U], 1.1F);
    ASSERT_FLOAT_EQ(a[1U], 2.2F);
    ASSERT_FLOAT_EQ(a[2U], 3.3F);

    ASSERT_EQ(a.size(), 3U);
}

TEST(TestList, insert_within_bounds)
{
    List<int> a{4, 6, 7};

    a.insert(5, 1U);

    ASSERT_EQ(a[0U], 4);
    ASSERT_EQ(a[1U], 5);
    ASSERT_EQ(a[2U], 6);
    ASSERT_EQ(a[3U], 7);

    ASSERT_EQ(a.size(), 4U);
}

TEST(TestList, insert_at_zero)
{
    List<int> a{4, 5};

    a.insert(3, 0U);

    ASSERT_EQ(a[0U], 3);
    ASSERT_EQ(a[1U], 4);
    ASSERT_EQ(a[2U], 5);

    ASSERT_EQ(a.size(), 3U);
}

TEST(TestList, insert_neg_index_within_bounds)
{
    List<float> a{1.0F, 2.0F, 3.0F, 4.0F};
    a.insert(-1.0F, -2);

    ASSERT_FLOAT_EQ(a[0U], 1.0F);
    ASSERT_FLOAT_EQ(a[1U], 2.0F);
    ASSERT_FLOAT_EQ(a[2U], -1.0F);
    ASSERT_FLOAT_EQ(a[3U], 3.0F);
    ASSERT_FLOAT_EQ(a[4U], 4.0F);

    ASSERT_EQ(a.size(), 5U);
}

TEST(TestList, insert_neg_index_out_of_bounds)
{
    List<float> a{1.0F, 2.0F, 3.0F, 4.0F};
    a.insert(-1.0F, -6);

    ASSERT_FLOAT_EQ(a[0U], 1.0F);
    ASSERT_FLOAT_EQ(a[1U], 2.0F);
    ASSERT_FLOAT_EQ(a[2U], -1.0F);
    ASSERT_FLOAT_EQ(a[3U], 3.0F);
    ASSERT_FLOAT_EQ(a[4U], 4.0F);

    ASSERT_EQ(a.size(), 5U);
}

TEST(TestList, insert_out_of_bounds)
{
    List<int> a{5, 6};

    a.insert(7, 100);

    ASSERT_EQ(a[0U], 5);
    ASSERT_EQ(a[1U], 6);
    ASSERT_EQ(a[2U], 7);

    ASSERT_EQ(a.size(), 3U);
}

// todo
// TEST(TestList, remove) { ASSERT_TRUE(false); }

}  // namespace core
}  // namespace kioku

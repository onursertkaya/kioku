#include "dstruct/buffer.h"
#include "dstruct/view.h"
#include <regex>
#include <vector>

#include "gtest/gtest.h"

using kioku::core::DynamicBuffer;
using kioku::core::DynamicView;
using kioku::core::StaticBuffer;
using kioku::core::View;

TEST(View, traits)
{
    constexpr bool same{std::is_same_v<View<float>::value_type, float>};
    ASSERT_TRUE(same);

    constexpr bool same_const_to_non_const{std::is_same_v<View<float const>::value_type, float>};
    ASSERT_TRUE(same_const_to_non_const);

    constexpr bool same_non_const_to_const{
        std::is_same_v<View<float>::const_value_type, float const>};
    ASSERT_TRUE(same_non_const_to_const);

    constexpr bool same_const_to_const{
        std::is_same_v<View<float const>::const_value_type, float const>};
    ASSERT_TRUE(same_const_to_const);
}

TEST(View, create_view)
{
    auto sb = StaticBuffer<int, 10>{};

    auto sb_view = createView(sb);
    auto const c_sb_view = createView(sb);

    ASSERT_EQ(sb_view.size(), sb.size());
    ASSERT_EQ(c_sb_view.size(), sb.size());

    auto db = DynamicBuffer<int>{5};

    auto db_view = createView(db);
    auto const c_db_view = createView(db);

    ASSERT_EQ(db_view.size(), db.size());
    ASSERT_EQ(c_db_view.size(), db.size());
}

TEST(View, create_const_view)
{
    auto sb = StaticBuffer<int, 10>{};

    auto sb_c_view = createConstView(sb);
    auto const c_sb_c_view = createConstView(sb);

    ASSERT_EQ(sb_c_view.size(), sb.size());
    ASSERT_EQ(c_sb_c_view.size(), sb.size());

    auto db = DynamicBuffer<int>{5};

    auto db_c_view = createConstView(db);
    auto const c_db_c_view = createConstView(db);

    ASSERT_EQ(db_c_view.size(), db.size());
    ASSERT_EQ(c_db_c_view.size(), db.size());
}

TEST(View, range_and_at)
{
    auto b = StaticBuffer<int, 10>{};
    b[0U] = 1;
    b[1U] = -2;
    b[2U] = 3;
    b[3U] = -4;
    b[4U] = 5;
    b[5U] = -6;
    b[6U] = 7;
    b[7U] = -8;
    b[8U] = 9;
    b[9U] = -10;

    auto b_view = createView(b);

    auto subview = b_view.range(3, 6);

    ASSERT_EQ(subview.size(), 3U);
    ASSERT_EQ(subview.at(0), -4);
    ASSERT_EQ(subview.at(1), 5);
    ASSERT_EQ(subview.at(2), -6);

    auto const c_sub_subview = subview.range(0, 2);
    ASSERT_EQ(c_sub_subview.size(), 2U);
    ASSERT_EQ(c_sub_subview.at(0), -4);
    ASSERT_EQ(c_sub_subview.at(1), 5);

    auto sub_sub_subview = c_sub_subview.range(0, 1);
    ASSERT_EQ(sub_sub_subview.size(), 1U);
    ASSERT_EQ(sub_sub_subview.at(0), -4);
}

TEST(View, at_access_const_correctness)
{
    auto sb = StaticBuffer<int, 3>{};
    sb[0] = 1;
    sb[1] = 2;
    sb[3] = 3;

    int* p_buffer = &sb[0];

    auto sb_view = createView(sb);
    int* p_view = &sb_view.at(0);

    // for view, ensure no return-by-value-occurs by checking the
    // equality of addresses.
    ASSERT_EQ(p_view, p_buffer);

    // for const view, ensure the same, but also ensure that the
    // returned reference is const, to guarantee that assignment by the
    // client is not allowed.
    auto sb_c_view = createConstView(sb);
    int const* p_c_view = &sb_c_view.at(0);
    ASSERT_EQ(p_c_view, p_buffer);

    using ConstViewAtReturnType = decltype(sb_c_view.at(0));
    constexpr bool same{std::is_same_v<ConstViewAtReturnType, int const&>};
    ASSERT_TRUE(same);
}

TEST(View_DeathTest, invalid_range)
{
    auto b = DynamicBuffer<int>{4};
    auto b_view = createView(b);

    ASSERT_DEATH({ static_cast<void>(b_view.range(0, 0)); }, ".*");
}

TEST(DynamicView, traits)
{
    constexpr bool same{std::is_same_v<DynamicView<float>::value_type, float>};
    ASSERT_TRUE(same);

    constexpr bool same_non_const_to_const{
        std::is_same_v<DynamicView<float>::const_value_type, float const>};
    ASSERT_TRUE(same_non_const_to_const);

    // float const as the type is prevented by static_assert in the class body.
    // constexpr bool same_const_to_non_const{std::is_same_v<DynamicView<float const>::value_type,
    // float>};
}

TEST(View, range_based_loop)
{
    auto b = StaticBuffer<int, 10>{};
    auto b_view = createView(b);

    std::vector<int> src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::size_t ctr = 0;
    for (auto& elem : b_view)
    {
        elem = src[ctr];
        ctr++;
    }

    b[0U] = 1;
    b[1U] = 2;
    b[2U] = 3;
    b[3U] = 4;
    b[4U] = 5;
    b[5U] = 6;
    b[6U] = 7;
    b[7U] = 8;
    b[8U] = 9;
    b[9U] = 10;
}

TEST(DynamicView, create)
{
    auto b = StaticBuffer<int, 5>{};
    auto a = createDynamicView(b);

    ASSERT_EQ(a.size(), 0U);
}

TEST(DynamicView, push_back)
{
    auto b = StaticBuffer<int, 5>{};
    auto a = createDynamicView(b);

    a.push_back(1);
    a.push_back(2);

    ASSERT_EQ(a.size(), 2U);

    a.push_back(3);
    a.push_back(4);
    a.push_back(5);

    ASSERT_EQ(a.size(), 5U);

    testing::internal::CaptureStdout();
    a.push_back(-1);
    std::string const output = testing::internal::GetCapturedStdout();

    std::string const regex_statement{std::string{"\\[DynamicView\\] push_back\\(\\): "}};

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);
}

TEST(DynamicView, pop_back)
{
    auto b = StaticBuffer<int, 5>{};
    auto a = createDynamicView(b);

    ASSERT_EQ(a.size(), 0U);

    a.push_back(4);
    ASSERT_EQ(a.size(), 1U);

    a.pop_back();
    ASSERT_EQ(a.size(), 0U);

    testing::internal::CaptureStdout();
    a.pop_back();
    std::string const output = testing::internal::GetCapturedStdout();

    std::string const regex_statement{std::string{"\\[DynamicView\\] pop_back\\(\\): "}};

    std::regex re{regex_statement.c_str(), std::regex_constants::ECMAScript};
    bool const match{std::regex_search(output, re)};
    ASSERT_TRUE(match);
}

TEST(View, at_access_const_correctness)
{
    auto sb = StaticBuffer<int, 3>{};
    sb[0] = 1;
    sb[1] = 2;
    sb[3] = 3;

    int* p_buffer = &sb[0];

    auto sb_view = createDynamicView(sb);
    int* p_view = &sb_view.at(0);

    // ensure no return-by-value-occurs by checking the
    // equality of addresses.
    ASSERT_EQ(p_view, p_buffer);

    using DynamicViewAtReturnType = decltype(sb_view.at(0));
    constexpr bool same{std::is_same_v<DynamicViewAtReturnType, int&>};
    ASSERT_TRUE(same);
}

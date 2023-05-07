extern "C"
{
#include "time_point.h"
}

#include <gtest/gtest.h>

TEST(time_point, TimeWithinBounds1ReturnsTrue)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 3, .minute = 39};
    time_point_t end          = {.hour = 3, .minute = 41};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, true);
}

TEST(time_point, TimeWithinBounds2ReturnsTrue)
{
    time_point_t time_checked = {.hour = 5, .minute = 59};
    time_point_t start        = {.hour = 1, .minute = 0};
    time_point_t end          = {.hour = 6, .minute = 0};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, true);
}

TEST(time_point, TimeOnStartBoundReturnsTrue)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 3, .minute = 40};
    time_point_t end          = {.hour = 5, .minute = 0};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, true);
}

TEST(time_point, TimeOnEndBoundReturnsTrue)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 1, .minute = 35};
    time_point_t end          = {.hour = 3, .minute = 40};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, true);
}

TEST(time_point, TimeOnTightBoundsReturnsTrue)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 3, .minute = 40};
    time_point_t end          = {.hour = 3, .minute = 40};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, true);
}

TEST(time_point, TimeOutsideBoundsReturnsFalse)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 3, .minute = 38};
    time_point_t end          = {.hour = 3, .minute = 39};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, false);
}
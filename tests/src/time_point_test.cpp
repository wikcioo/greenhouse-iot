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

TEST(time_point, TimeOnEndBoundReturnsFalse)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 1, .minute = 35};
    time_point_t end          = {.hour = 3, .minute = 40};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, false);
}

TEST(time_point, TimeOnTightBoundsReturnsFalse)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 3, .minute = 40};
    time_point_t end          = {.hour = 3, .minute = 40};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, false);
}

TEST(time_point, TimeOutsideBoundsReturnsFalse)
{
    time_point_t time_checked = {.hour = 3, .minute = 40};
    time_point_t start        = {.hour = 3, .minute = 38};
    time_point_t end          = {.hour = 3, .minute = 39};

    bool result = time_is_between(&time_checked, &start, &end);

    EXPECT_EQ(result, false);
}

TEST(time_point, EarlierTimeIsBeforeLaterTime)
{
    time_point_t before = {.hour = 8, .minute = 40};
    time_point_t after  = {.hour = 9, .minute = 0};

    bool result = time_is_before(&before, &after);

    EXPECT_EQ(result, true);
}

TEST(time_point, LaterTimeIsNotBeforeEarlierTime)
{
    time_point_t before = {.hour = 8, .minute = 40};
    time_point_t after  = {.hour = 7, .minute = 40};

    bool result = time_is_before(&before, &after);

    EXPECT_EQ(result, false);
}

TEST(time_point, SameTimeIsNotBeforeOther)
{
    time_point_t before = {.hour = 8, .minute = 40};
    time_point_t after  = {.hour = 8, .minute = 40};

    bool result = time_is_before(&before, &after);

    EXPECT_EQ(result, false);
}

TEST(time_point, DiffInTimeNormal1)
{
    time_point_t t1 = {.hour = 8, .minute = 40};
    time_point_t t2 = {.hour = 9, .minute = 45};

    uint16_t result = time_get_diff_in_minutes(&t1, &t2);

    EXPECT_EQ(result, 65);
}

TEST(time_point, DiffInTimeNormal2)
{
    time_point_t t1 = {.hour = 7, .minute = 10};
    time_point_t t2 = {.hour = 12, .minute = 19};

    uint16_t result = time_get_diff_in_minutes(&t1, &t2);

    EXPECT_EQ(result, 309);
}

TEST(time_point, DiffInTimeT1AfterT2)
{
    time_point_t t1 = {.hour = 11, .minute = 10};
    time_point_t t2 = {.hour = 10, .minute = 45};

    uint16_t result = time_get_diff_in_minutes(&t1, &t2);

    EXPECT_EQ(result, 25);
}

TEST(time_point, DiffInTimeSameTime)
{
    time_point_t t1 = {.hour = 8, .minute = 40};
    time_point_t t2 = {.hour = 8, .minute = 40};

    uint16_t result = time_get_diff_in_minutes(&t1, &t2);

    EXPECT_EQ(result, 0);
}

TEST(time_point, DiffUntilMidnight1)
{
    time_point_t t1 = {.hour = 20, .minute = 40};
    time_point_t t2 = {.hour = 24, .minute = 0};

    uint16_t result = time_get_diff_in_minutes(&t1, &t2);

    EXPECT_EQ(result, 200);
}

TEST(time_point, DiffUntilMidnight2)
{
    time_point_t t1 = {.hour = 5, .minute = 16};
    time_point_t t2 = {.hour = 24, .minute = 0};

    uint16_t result = time_get_diff_in_minutes(&t1, &t2);

    EXPECT_EQ(result, 1124);
}
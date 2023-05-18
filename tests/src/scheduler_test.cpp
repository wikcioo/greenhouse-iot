extern "C"
{
#include "scheduler.h"
}

#include <gtest/gtest.h>

#include "FreeRTOS_FFF_MocksDeclaration.h"
#include "fff.h"

extern interval_info_t interval_info;
extern time_point_t    daily_time;
extern bool            water_valve_state;

class SchedulerTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        RESET_FAKE(xMessageBufferReceive);
        RESET_FAKE(xTaskCreate);
        RESET_FAKE(xTaskDelayUntil);
        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
};

TEST_F(SchedulerTest, scheduler_handler_initialise)
{
    scheduler_handler_initialise(1, 3, 3);

    ASSERT_EQ(xTaskCreate_fake.call_count, 3);
}

interval_t example_interval;

size_t xMessageBufferReceiveCustomFake(MessageBufferHandle_t buffer, void *data, size_t size, TickType_t delay)
{
    interval_t *interval_ptr = (interval_t *) data;

    *interval_ptr = example_interval;

    return 0;
}

TEST_F(SchedulerTest, receive_data_handler_task_run_normal_interval)
{
    example_interval = (interval_t){.start = {.hour = 8, .minute = 30}, .end = {.hour = 9, .minute = 45}};

    xMessageBufferReceive_fake.custom_fake = xMessageBufferReceiveCustomFake;

    scheduler_receive_data_handler_task_run();

    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);

    ASSERT_EQ(interval_info.current_size, 1);
    ASSERT_EQ(interval_info.intervals[0].start.hour, example_interval.start.hour);
    ASSERT_EQ(interval_info.intervals[0].start.minute, example_interval.start.minute);
    ASSERT_EQ(interval_info.intervals[0].end.hour, example_interval.end.hour);
    ASSERT_EQ(interval_info.intervals[0].end.minute, example_interval.end.minute);
}

TEST_F(SchedulerTest, receive_data_handler_task_run_zeroed_interval)
{
    example_interval = (interval_t){.start = {.hour = 0, .minute = 0}, .end = {.hour = 0, .minute = 0}};

    xMessageBufferReceive_fake.custom_fake = xMessageBufferReceiveCustomFake;

    scheduler_receive_data_handler_task_run();

    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(interval_info.current_size, 0);
}

TEST_F(SchedulerTest, schedule_events_handler_task_run_daily_time_inside_interval_array)
{
    daily_time = {.hour = 6, .minute = 0};

    interval_info.current_size = 1;
    interval_info.intervals[0] = {.start = {.hour = 5, .minute = 55}, .end = {.hour = 6, .minute = 5}};

    scheduler_schedule_events_handler_task_run();

    ASSERT_EQ(xTaskDelayUntil_fake.call_count, 1);
    // Sleep until 6:00 => 5 minutes = 300_000 millis
    ASSERT_EQ(xTaskDelayUntil_fake.arg1_val, 300000);
}

TEST_F(SchedulerTest, schedule_events_handler_task_run_daily_time_outside_interval_array)
{
    daily_time = {.hour = 5, .minute = 55};

    interval_info.current_size = 1;
    interval_info.intervals[0] = {.start = {.hour = 5, .minute = 50}, .end = {.hour = 5, .minute = 55}};

    scheduler_schedule_events_handler_task_run();

     ASSERT_EQ(xTaskDelayUntil_fake.call_count, 1);
    //  Sleep until midnight => 1085 minutes = 65100000 millis
    ASSERT_EQ(xTaskDelayUntil_fake.arg1_val, 65100000);
}
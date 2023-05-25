extern "C"
{
#include "hardware_controller.h"

#include "water_controller.h"
}

#include <gtest/gtest.h>

#include "FreeRTOS_FFF_MocksDeclaration.h"

extern range_t temp_range;
extern range_t hum_range;
extern range_t co2_range;

extern bool is_water_valve_open;

extern time_point_t daily_time;
extern time_point_t end_watering_time;

extern MessageBufferHandle_t upLinkMessageBufferHandle;

class HardwareControllerTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // FreeRTOS fff's
        RESET_FAKE(xTaskCreate);
        RESET_FAKE(xMessageBufferReceive);
        RESET_FAKE(xMessageBufferSend);
        RESET_FAKE(xEventGroupWaitBits);
        RESET_FAKE(xTaskDelayUntil);
        RESET_FAKE(vTaskDelay);
        RESET_FAKE(xTaskGetTickCount);
        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
};

TEST_F(HardwareControllerTest, hc_handler_initialise)
{
    hc_handler_initialise(3, 3, 3);

    ASSERT_EQ(xTaskCreate_fake.call_count, 3);
}

preset_data_t example_presets;

size_t hc_xMessageBufferReceiveCustomFake(MessageBufferHandle_t buffer, void *data, size_t size, TickType_t delay)
{
    preset_data_t *presets_ptr = (preset_data_t *) data;

    *presets_ptr = example_presets;

    return 0;
}

TEST_F(HardwareControllerTest, hc_receive_preset_data_handler_task_run)
{
    range_t *temp_range_data = (range_t *) malloc(sizeof(range_t));
    range_t *hum_range_data  = (range_t *) malloc(sizeof(range_t));
    range_t *co2_range_data  = (range_t *) malloc(sizeof(range_t));

    temp_range_data->low  = 700;
    temp_range_data->high = 820;

    hum_range_data->low  = 200;
    hum_range_data->high = 700;

    co2_range_data->low  = 200;
    co2_range_data->high = 1500;

    example_presets =
        (preset_data_t){.temp_range = temp_range_data, .hum_range = hum_range_data, .co2_range = co2_range_data};

    xMessageBufferReceive_fake.custom_fake = hc_xMessageBufferReceiveCustomFake;

    hc_receive_preset_data_handler_task_run();

    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(temp_range.low, 700);
    ASSERT_EQ(temp_range.high, 820);
    ASSERT_EQ(hum_range.low, 2000);
    ASSERT_EQ(hum_range.high, 7000);
    ASSERT_EQ(co2_range.low, 200);
    ASSERT_EQ(co2_range.high, 1500);

    ASSERT_EQ(vTaskDelay_fake.call_count, 1);
    ASSERT_EQ(vTaskDelay_fake.arg0_val, pdMS_TO_TICKS(2000));

    free(temp_range_data);
    free(hum_range_data);
    free(co2_range_data);
}

action_t example_action;

size_t hc_xMessageBufferReceiveActionCustomFake(MessageBufferHandle_t buffer, void *data, size_t size, TickType_t delay)
{
    action_t *action_ptr = (action_t *) data;

    *action_ptr = example_action;

    return 0;
}

TEST_F(HardwareControllerTest, hc_toggle_handler_task_run_water_off)
{
    example_action = (action_t){.water_on = false};

    xMessageBufferReceive_fake.custom_fake = hc_xMessageBufferReceiveActionCustomFake;

    hc_toggle_handler_task_run();

    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(action_t));

    ASSERT_EQ(is_water_valve_open, false);
}

TEST_F(HardwareControllerTest, hc_toggle_handler_task_run_water_on_keep_end_watering_time)
{
    daily_time.hour   = 5;
    daily_time.minute = 10;

    end_watering_time.hour   = 5;
    end_watering_time.minute = 30;

    example_action = (action_t){.water_on = true, .duration = 10};

    xMessageBufferReceive_fake.custom_fake = hc_xMessageBufferReceiveActionCustomFake;

    hc_toggle_handler_task_run();

    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(action_t));

    ASSERT_EQ(is_water_valve_open, true);
    ASSERT_EQ(end_watering_time.hour, 5);
    ASSERT_EQ(end_watering_time.minute, 30);
}

TEST_F(HardwareControllerTest, hc_toggle_handler_task_run_water_on_increment_end_watering_time)
{
    daily_time.hour   = 5;
    daily_time.minute = 10;

    end_watering_time.hour   = 5;
    end_watering_time.minute = 30;

    example_action = (action_t){.water_on = true, .duration = 30};

    xMessageBufferReceive_fake.custom_fake = hc_xMessageBufferReceiveActionCustomFake;

    hc_toggle_handler_task_run();

    ASSERT_EQ(xMessageBufferReceive_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferReceive_fake.arg2_val, sizeof(action_t));

    ASSERT_EQ(is_water_valve_open, true);
    ASSERT_EQ(end_watering_time.hour, 5);
    ASSERT_EQ(end_watering_time.minute, 40);
}

TEST_F(HardwareControllerTest, hc_handler_task_run1)
{
    hc_measurement_handler_task_run(5);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 1);
    ASSERT_EQ(xMessageBufferSend_fake.arg0_val, upLinkMessageBufferHandle);
    ASSERT_EQ(xMessageBufferSend_fake.arg2_val, sizeof(sensor_data_t));
    ASSERT_EQ(xMessageBufferSend_fake.arg3_val, portMAX_DELAY);
}

TEST_F(HardwareControllerTest, hc_handler_task_run2)
{
    hc_measurement_handler_task_run(0);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 0);
}

TEST_F(HardwareControllerTest, hc_handler_task_run3)
{
    hc_measurement_handler_task_run(4);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 0);
}

TEST_F(HardwareControllerTest, hc_handler_task_run4)
{
    hc_measurement_handler_task_run(6);

    ASSERT_EQ(xMessageBufferSend_fake.call_count, 0);
}
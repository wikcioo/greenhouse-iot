extern "C"
{
#include <hih8120.h>
#include <stdint.h>

#include "humidity_temperature.h"
}

#include <gtest/gtest.h>

#include "fff.h"

extern uint16_t hum_last_measurement;

FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_initialise);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_wakeup);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_measure);

FAKE_VALUE_FUNC(uint16_t, hih8120_getHumidityPercent_x10);
FAKE_VALUE_FUNC(int16_t, hih8120_getTemperature_x10);

FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_destroy);

class HumidityTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        RESET_FAKE(hih8120_initialise);
        RESET_FAKE(hih8120_wakeup);
        RESET_FAKE(hih8120_measure);
        RESET_FAKE(hih8120_getHumidityPercent_x10);
        RESET_FAKE(hih8120_getTemperature_x10);
        RESET_FAKE(hih8120_destroy);
        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
};

TEST_F(HumidityTest, humidity_init)
{
    hum_temp_init();

    ASSERT_EQ(hih8120_initialise_fake.call_count, 1);
}

TEST_F(HumidityTest, humidity_destroy)
{
    hum_temp_destroy();

    ASSERT_EQ(hih8120_destroy_fake.call_count, 1);
    ASSERT_EQ(hih8120_destroy_fake.return_val, HIH8120_OK);
}

TEST_F(HumidityTest, get_last_humidity_measurement)
{
    hum_last_measurement = 225;

    uint16_t result = get_last_humidity_measurement();

    ASSERT_EQ(result, 225);
}

TEST_F(HumidityTest, humidity_measure)
{
    hih8120_getHumidityPercent_x10_fake.return_val = 225;

    bool status = hum_temp_measure();

    ASSERT_EQ(hih8120_wakeup_fake.call_count, 1);
    ASSERT_EQ(hih8120_wakeup_fake.return_val, HIH8120_OK);

    ASSERT_EQ(hih8120_measure_fake.call_count, 1);
    ASSERT_EQ(hih8120_measure_fake.return_val, HIH8120_OK);

    ASSERT_EQ(hih8120_getHumidityPercent_x10_fake.call_count, 1);
    ASSERT_EQ(hih8120_getHumidityPercent_x10_fake.return_val, 225);

    ASSERT_EQ(status, true);
}


TEST_F(HumidityTest, humidity_measure_wakeup_fail)
{
    hih8120_wakeup_fake.return_val =  HIH8120_DRIVER_NOT_INITIALISED;

    bool status = hum_temp_measure();

    ASSERT_EQ(hih8120_wakeup_fake.call_count, 1);
    ASSERT_EQ(hih8120_wakeup_fake.return_val, HIH8120_DRIVER_NOT_INITIALISED);

    ASSERT_EQ(hih8120_measure_fake.call_count, 0);

    ASSERT_EQ(hih8120_getHumidityPercent_x10_fake.call_count, 0);

    ASSERT_EQ(status, false);
}
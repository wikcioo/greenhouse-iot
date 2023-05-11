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
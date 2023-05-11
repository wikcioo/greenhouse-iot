extern "C"
{
#include <hih8120.h>

#include "humidity_temperature.h"
}

#include <gtest/gtest.h>

#include "fff.h"

extern uint16_t hum_last_measurement;

FAKE_VOID_FUNC(hih8120_initialise);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_wakeup);
FAKE_VALUE_FUNC(hih8120_driverReturnCode_t, hih8120_measure);

class HumidityTest : public ::testing::TEST
{
   protected:
    void SetUp() override
    {
        RESET_FAKE(hih8120_initialise);
        RESET_FAKE(hih8120_wakeup);
        RESET_FAKE(hih8120_measure);
        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
}
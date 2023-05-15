extern "C"
{
#include "water_controller.h"

#include <rc_servo.h>
}

#include <gtest/gtest.h>

#include "fff.h"

extern bool water_valve_state;

FAKE_VOID_FUNC(rc_servo_initialise);
FAKE_VOID_FUNC(rc_servo_setPosition, uint8_t, int8_t);

class WaterController : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        RESET_FAKE(rc_servo_initialise);
        RESET_FAKE(rc_servo_setPosition);
        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
};

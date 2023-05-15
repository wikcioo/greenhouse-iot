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


TEST_F(WaterController, water_controller_init)
{
    water_controller_init();

    ASSERT_EQ(rc_servo_initialise_fake.call_count, 1);
    ASSERT_EQ(water_valve_state, false);
}

TEST_F(WaterController, water_controller_on)
{
    water_controller_on();

    ASSERT_EQ(rc_servo_setPosition_fake.call_count, 1);
    ASSERT_EQ(water_valve_state, true);
}

TEST_F(WaterController, water_controller_off)
{
    water_controller_off();

    ASSERT_EQ(rc_servo_setPosition_fake.call_count, 1);
    ASSERT_EQ(water_valve_state, false);
}

TEST_F(WaterController, water_controller_get_state)
{
    water_valve_state = true;

    bool result = water_controller_get_state();

    ASSERT_EQ(water_valve_state, true);
}
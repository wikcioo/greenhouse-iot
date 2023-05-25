extern "C"
{
#include "water_controller.h"

#include <rc_servo.h>
}

#include <gtest/gtest.h>

#include "fff.h"

extern bool is_water_valve_open;

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
    ASSERT_EQ(is_water_valve_open, false);
}

TEST_F(WaterController, water_controller_on)
{
    water_controller_on();

    ASSERT_EQ(rc_servo_setPosition_fake.call_count, 1);
    ASSERT_EQ(is_water_valve_open, true);
}

TEST_F(WaterController, water_controller_off)
{
    water_controller_off();

    ASSERT_EQ(rc_servo_setPosition_fake.call_count, 1);
    ASSERT_EQ(is_water_valve_open, false);
}

TEST_F(WaterController, water_controller_get_state)
{
    is_water_valve_open = true;

    bool result = water_controller_is_water_valve_open();

    ASSERT_EQ(is_water_valve_open, true);
}
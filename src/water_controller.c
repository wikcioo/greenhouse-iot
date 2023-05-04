#include "water_controller.h"

#include <rc_servo.h>

bool water_valve_state;

void water_controller_init()
{
    rc_servo_initialise();
}

void water_controller_on()
{
    rc_servo_setPosition(1, 100);
    water_valve_state = true;
}

void water_controller_off()
{
    rc_servo_setPosition(1, 0);
    water_valve_state = false;
}
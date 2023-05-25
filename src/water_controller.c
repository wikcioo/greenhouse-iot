#include "water_controller.h"

#include <rc_servo.h>

bool is_water_valve_open;

void water_controller_init()
{
    rc_servo_initialise();

    // In case this is not performed on initialisation
    water_controller_off();
}

void water_controller_on()
{
    rc_servo_setPosition(1, 100);
    is_water_valve_open = true;
}

void water_controller_off()
{
    rc_servo_setPosition(1, 0);
    is_water_valve_open = false;
}

bool water_controller_is_water_valve_open()
{
    return is_water_valve_open;
}
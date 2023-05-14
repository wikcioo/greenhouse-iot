#include <hih8120.h>
#include <stdint.h>

#ifndef TEST_ENV
#include <util/delay.h>
#else
void _delay_ms(double) {}
#endif

#include "humidity_temperature.h"

uint16_t hum_last_measurement, temp_last_measurement;

bool hum_temp_init()
{
    if (HIH8120_OK == hih8120_initialise())
    {
        return true;
    };

    return false;
}

bool hum_temp_measure()
{
    if (HIH8120_OK != hih8120_wakeup())
    {
        return false;
    }

    _delay_ms(51);

    if (HIH8120_OK == hih8120_measure())
    {
        _delay_ms(2);

        hum_last_measurement  = hih8120_getHumidityPercent_x10();
        temp_last_measurement = hih8120_getTemperature_x10() + 500;

        return true;
    }

    if (HIH8120_TWI_BUSY == hih8120_measure())
    {
        _delay_ms(2);

        if (HIH8120_OK == hih8120_measure())
        {
            _delay_ms(2);

            hum_last_measurement  = hih8120_getHumidityPercent_x10();
            temp_last_measurement = hih8120_getTemperature_x10() + 500;

            return true;
        }
    }

    return false;
}

void hum_temp_destroy()
{
    hih8120_destroy();
}

uint16_t get_last_humidity_measurement()
{
    return hum_last_measurement;
}

uint16_t get_last_temperature_measurement()
{
    return temp_last_measurement;
}
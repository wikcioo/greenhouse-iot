#include "humidity_temperature.h"

#include <hih8120.h>
#include <util/delay.h>

bool hum_temp_init()
{
    if (HIH8120_OK == hih8120_initialise())
    {
        return true;
    };
    return false;
}

bool hum_temp_measure(uint16_t *hume_buffer, uint16_t *temp_buffer)
{
    if (HIH8120_OK != hih8120_wakeup())
    {
        return false;
    }
    _delay_ms(51);
    if (HIH8120_OK == hih8120_measure())
    {
        _delay_ms(1);

        *hume_buffer = hih8120_getHumidityPercent_x10();
        _delay_ms(1);

        *temp_buffer = hih8120_getTemperature_x10() + 500;
        return true;
    }
    if (HIH8120_TWI_BUSY == hih8120_measure())
    {
        _delay_ms(2);

        if (HIH8120_OK == hih8120_measure())
        {
            _delay_ms(2);

            *hume_buffer = hih8120_getHumidityPercent_x10();
            *temp_buffer = hih8120_getTemperature_x10() + 500;
            return true;
        }
    }
    return false;
}

void hum_temp_destroy()
{
    hih8120_destroy();
}

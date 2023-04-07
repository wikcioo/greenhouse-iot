#include <hih8120.h>

#include "humidity_temperature.h"


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
    if (hum_temp_init() & HIH8120_OK == hih8120_measure())
    {
        *hume_buffer = hih8120_getHumidityPercent_x10();
        *temp_buffer = hih8120_getTemperature_x10() + 500;
        hum_temp_destroy();
        return true;
    }
    if (HIH8120_TWI_BUSY == hih8120_measure())
    {
        sleep(1);
        if (HIH8120_OK == hih8120_measure())
        {
            *hume_buffer = hih8120_getHumidityPercent_x10();
            *temp_buffer = hih8120_getTemperature_x10() + 500;
            hum_temp_destroy();
            return true;
        }
    }
    hum_temp_destroy();
    return false;
}

void hum_temp_destroy()
{
    hih8120_destroy();
}

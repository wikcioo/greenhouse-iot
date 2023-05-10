#include <mh_z19.h>
#include <stdio.h>

#include "co2.h"

#define CO2_RET_CODE_TO_NAME(x) (x == 0 ? "OK" : x == 1 ? "NO MEASUREMENT" : x == 2 ? "NO SERIAL" : "PPM MUST BE > 99")

uint16_t co2_last_read;

void co2_callback(uint16_t value)
{
    co2_last_read = value;
}

bool co2_init()
{
    mh_z19_initialise(ser_USART3);
    mh_z19_injectCallBack(co2_callback);

#ifdef IS_CO2_OUTDOORS
    mh_z19_returnCode_t status = mh_z19_setAutoCalibration(true);
    return status == MHZ19_OK;
#endif

    return true;
}

bool co2_measure()
{
    mh_z19_returnCode_t status = mh_z19_takeMeassuring();
    return status == MHZ19_OK;
}

uint16_t co2_get_latest_measurement()
{
    return co2_last_read;
}
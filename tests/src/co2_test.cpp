extern "C"
{
#include "co2.h"

#include <mh_z19.h>
}

#include <gtest/gtest.h>

#include "fff.h"

extern uint16_t co2_last_read;

FAKE_VOID_FUNC(mh_z19_initialise, serial_comPort_t);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);

typedef void (*mh_z19_callback)(uint16_t);
FAKE_VOID_FUNC(mh_z19_injectCallBack, mh_z19_callback);

class Co2Test : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        RESET_FAKE(mh_z19_initialise);
        RESET_FAKE(mh_z19_takeMeassuring);
        RESET_FAKE(mh_z19_injectCallBack);
        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
};

TEST_F(Co2Test, co2_init)
{
    co2_init();

    ASSERT_EQ(mh_z19_initialise_fake.call_count, 1);
    ASSERT_EQ(mh_z19_initialise_fake.arg0_val, ser_USART3);
    ASSERT_EQ(mh_z19_injectCallBack_fake.call_count, 1);
}

TEST_F(Co2Test, co2_measure)
{
    mh_z19_takeMeassuring_fake.return_val = MHZ19_OK;

    bool status = co2_measure();

    ASSERT_EQ(mh_z19_takeMeassuring_fake.call_count, 1);
    ASSERT_EQ(status, true);
}

uint16_t _co2_last_read = 555;

void inject_custom_callback_fake(void (*mh_z19_callback)(uint16_t))
{
    if (mh_z19_callback)
        mh_z19_callback(_co2_last_read);
}

TEST_F(Co2Test, co2_callback)
{
    mh_z19_injectCallBack_fake.custom_fake = inject_custom_callback_fake;

    co2_init();

    ASSERT_EQ(mh_z19_injectCallBack_fake.call_count, 1);
    ASSERT_EQ(co2_last_read, _co2_last_read);
}

TEST_F(Co2Test, co2_get_latest_measurement)
{
    co2_last_read = 234;

    uint16_t result = co2_get_latest_measurement();

    ASSERT_EQ(result, 234);
}
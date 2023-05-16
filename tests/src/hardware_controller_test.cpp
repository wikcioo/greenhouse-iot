extern "C"
{
#include "hardware_controller.h"
}

#include <gtest/gtest.h>

#include "FreeRTOS_FFF_MocksDeclaration.h"

class HardwareControllerTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // FreeRTOS fff's
        RESET_FAKE(xTaskCreate);
        RESET_FAKE(xMessageBufferReceive);
        RESET_FAKE(xEventGroupWaitBits);
        RESET_FAKE(xTaskDelayUntil);
        RESET_FAKE(vTaskDelay);
        RESET_FAKE(xTaskGetTickCount);
        FFF_RESET_HISTORY();
    }

    void TearDown() override {}
};

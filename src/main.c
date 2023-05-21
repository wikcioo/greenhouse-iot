#include <stdio.h>

#include "initialiser.h"
#include "logger.h"

int main(void)
{
    initialiseSystem();
    LOG("Program Started!!\n");

    vTaskStartScheduler();

    while (1)
    {
    }
}
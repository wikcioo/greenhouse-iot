#include <stdio.h>

#include "manager.h"

int main(void)
{
    initialiseManager();
    printf("Program Started!!\n");

    vTaskStartScheduler();

    while (1)
    {
    }
}
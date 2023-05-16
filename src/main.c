#include <stdio.h>

#include "initialiser.h"

int main(void)
{
    initialiseSystem();
    printf("Program Started!!\n");

    vTaskStartScheduler();

    while (1)
    {
    }
}
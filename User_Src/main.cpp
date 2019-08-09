#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "usart.h"
#include <cstdio>

#include "tim.h"
#include "Dimmer.h"



extern "C" {
void SystemClock_Config();
}


void init();

int main() {
    init();

    //TODO use real data

    Dimmer::init();
    Dimmer::setMinBrightness(50);
    Dimmer::setMaxBrightness(250);

    uint8_t val = 0;

    while(true)
    {
        HAL_Delay(10);
        Dimmer::setDimmerLevel(0, val);
        ++val;
        //printf("val: %d\n", 255);

    }
}


void init()
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    //MX_DMA_Init();
    //MX_USART3_UART_Init();
    MX_TIM3_Init();
    printf("Startup complete\n");
}


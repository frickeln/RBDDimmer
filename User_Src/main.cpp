#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "usart.h"
#include <cstdio>

#include "Pin.h"
#include "Dimmer.h"



extern "C" {
void SystemClock_Config();
}


void init();


int main() {
    init();

    //TODO use real data
    Pin pins[4] = {Pin{1, 1}, Pin{1, 1}, Pin{1, 1}, Pin{1, 1}};

    Dimmer testDimmer{pins[0], pins[1], pins[2], pins[3]};

    while(true)
    {
	printf("bla\n");
    }
}


void init()
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    //MX_DMA_Init();
    MX_USART1_UART_Init();
    //MX_USART3_UART_Init();
    //MX_TIM3_Init();
    printf("Startup complete\n");
}


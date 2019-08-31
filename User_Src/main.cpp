#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "usart.h"
#include <cstdio>
#include "dma.h"
#include "tim.h"
#include "Dimmer.h"
#include "DmxReceiver.h"



extern "C" {
void SystemClock_Config();
}


void init();

volatile bool frameReceived = false;
int dmxAddr = 1;

int main() {

    init();

    //TODO read dmx addr from pins and limit to reasonable values
    //TODO input values 0 -> 0, 1 -> minimum
    //TODO indicate active dmx using blinking led

    while(true)
    {
        if(frameReceived) {
            frameReceived = false;
            for (int i = 0; i < Dimmer::NUM_DIMMERS; ++i) {
                Dimmer::setDimmerLevel(i, DmxReceiver::buffer[dmxAddr + i]);
            }
        }
    }
}

void dmxFrameReceived(const uint8_t* buffer)
{
    //we dont care about the buffer, main access it directly
    frameReceived = true;
}


void init()
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_DMA_Init();
    MX_USART3_UART_Init();
    MX_TIM3_Init();
    DmxReceiver::init(dmxFrameReceived);
    Dimmer::init();
    Dimmer::setMinBrightness(40);
    Dimmer::setMaxBrightness(250);
    printf("Startup complete\n");
}


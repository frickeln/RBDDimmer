//
// Created by arne on 05.08.19.
//

#include <cstdio>
#include "Dimmer.h"
#include "tim.h"
#include "gpio.h"

uint8_t Dimmer::dimmerLevels[NUM_DIMMER_PINS] = {255};
uint16_t Dimmer::currentTick = 0;
const Pin Dimmer::pins[NUM_DIMMER_PINS] = {Pin{GPIOB, GPIO_PIN_12}, Pin{GPIOB, GPIO_PIN_13}, Pin{GPIOB, GPIO_PIN_14}, Pin{GPIOB, GPIO_PIN_15}};

void Dimmer::init() {

    //zero crossing interrupt appears every 10ms (100hz)
    //one timer overflow should be 0.01s/256 = 0.000039062s, The timer should tick 74 times (small error at 72mhz) times before overflowing
    //thus one timer tick should take 0,000000528s
}

int setTick[4] = {300};

void Dimmer::notifyZeroCrossing() {
    //exti has less priority than timer tick. Thus when we are here we can be sure that no timer is running
    //stop the timer to be able to reset state without race conditions
    HAL_TIM_Base_Stop_IT(&htim3);

    currentTick = 0;

    //turn all lamps off on zero crossing
    for(int i = 0; i < NUM_DIMMER_PINS; ++i) {
        setTick[i] = 300;
        //turn all lamps on for at least one tick unless dimmer level is at 100%
        if(dimmerLevels[i] != 0)
        {
            HAL_GPIO_WritePin(pins[i].port, pins[i].pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(pins[i].port, pins[i].pin, GPIO_PIN_SET);
        }
    }

    HAL_TIM_Base_Start_IT(&htim3);
}




void Dimmer::notifyTimerTick() {
    //if timer is setup correctly whe should have exactly 255 ticks between zero crossings
    //TODO verify timing!
    static const uint8_t maxTicks = 255;

    ++currentTick;

    if(currentTick >= maxTicks)
    {
        HAL_TIM_Base_Stop_IT(&htim3);
        return;
    }

    //tick 1 => 0,000039062s have already passed
    // tick >= 255 => turn off timer (will be turned back on at next zero crossing) (break function without turning on lamp)

    // tick >= (255 - dmx value) => turn on

    for(int i = 0; i < NUM_DIMMER_PINS; ++i) {
        if(currentTick >= (maxTicks - dimmerLevels[i]) && setTick[i] == 300)
        {
            HAL_GPIO_WritePin(pins[i].port, pins[i].pin, GPIO_PIN_SET);
            setTick[i] = currentTick;
        }
        if(currentTick > setTick[i] + 3)
        {
            HAL_GPIO_WritePin(pins[i].port, pins[i].pin, GPIO_PIN_RESET);
        }

    }
}



void Dimmer::setPinOn(const Pin &pin, bool on) {
//TODO remove dummy implementation
}

void Dimmer::setDimmerLevel(int i, uint8_t level) {
    if(i >= 0 && i < NUM_DIMMER_PINS)
    {
        dimmerLevels[i] = level;
    }
    else
    {
        printf("dimmer number out of range");
    }
}

extern "C"
{
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
    {
        Dimmer::notifyZeroCrossing();
    }

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance==TIM3)
    {
        Dimmer::notifyTimerTick();
    }
}

}


//
// Created by arne on 05.08.19.
//

#include <cstdio>
#include "Dimmer.h"
#include "tim.h"
#include "gpio.h"

#define LAST_PULSE_RESET -1
#define PULSE_LENGTH 20

uint8_t Dimmer::dimmerLevels[NUM_DIMMER_PINS] = {255};
uint16_t Dimmer::currentTick = 0;
const Pin Dimmer::pins[NUM_DIMMER_PINS] = {Pin{GPIOB, GPIO_PIN_12}, Pin{GPIOB, GPIO_PIN_13}, Pin{GPIOB, GPIO_PIN_14}, Pin{GPIOB, GPIO_PIN_15}};
int16_t Dimmer::lastPulseTime[NUM_DIMMER_PINS] = {LAST_PULSE_RESET};
uint8_t Dimmer::minBrightness = 0;
uint8_t Dimmer::maxBrightness = 255;
bool Dimmer::timerLock= false;

void Dimmer::init() {

    //zero crossing interrupt appears every 10ms (100hz)
    //one timer overflow should be 0.01s/256 = 0.000039062s, The timer should tick 74 times (small error at 72mhz) times before overflowing
    //thus one timer tick should take 0,000000528s
    currentTick = 100; //start at non zero value to enter first zero crossing detection
    HAL_TIM_Base_Start_IT(&htim3);
}

long bla = 0;

void Dimmer::notifyZeroCrossing() {

    if (HAL_GetTick() - bla < 3)
        return;
    bla = HAL_GetTick();


    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

    //exti has less priority than timer tick. Thus when we are here we can be sure that no timer is running
    //stop the timer to be able to reset state without race conditions
    HAL_TIM_Base_Stop_IT(&htim3);
    currentTick = 0;

    //Lamps turn off on zero crossing automatically
    //end all pulses (some pulses might still be running)
    for(const Pin& p : pins)
    {
        HAL_GPIO_WritePin(p.port, p.pin, GPIO_PIN_RESET);
    }

    //reset pulse start times
    for(int i = 0; i < NUM_DIMMER_PINS; ++i)
    {
        lastPulseTime[i] = LAST_PULSE_RESET;
    }
    HAL_TIM_Base_Start_IT(&htim3);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}




void Dimmer::notifyTimerTick() {

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);


    //tick 1 => 0,000039062s have already passed
    // tick >= 255 => turn off timer (will be turned back on at next zero crossing) (break function without turning on lamp)

    // tick >= (255 - dmx value) => turn on

    static const uint8_t maxTicks = 255;

    ++currentTick;

    if(currentTick >= maxTicks)
    {
        //end all pulses (if pulses started at e.g. 254 they would otherwise run forever (causing flickering).
        for(const Pin& p : pins)
        {
            HAL_GPIO_WritePin(p.port, p.pin, GPIO_PIN_RESET);
        }
        HAL_TIM_Base_Stop_IT(&htim3);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
        return;
    }


    for(int i = 0; i < NUM_DIMMER_PINS; ++i) {

        if(currentTick >= (maxTicks - dimmerLevels[i])
           && lastPulseTime[i] == LAST_PULSE_RESET) //only turn on if it hasn't been turned on before
        {
            //start pulse
            HAL_GPIO_WritePin(pins[i].port, pins[i].pin, GPIO_PIN_SET);
            lastPulseTime[i] = currentTick;
        }

        //end pulse PULSE_LENGTH ticks after start pulse
        if(currentTick > lastPulseTime[i] + PULSE_LENGTH && lastPulseTime[i] != LAST_PULSE_RESET)
        {
            HAL_GPIO_WritePin(pins[i].port, pins[i].pin, GPIO_PIN_RESET);
        }

    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}



void Dimmer::setPinOn(const Pin &pin, bool on) {
//TODO remove dummy implementation
}

void Dimmer::setDimmerLevel(int i, uint8_t level) {
    if(level < minBrightness)
        level = 0;

    if(level > maxBrightness)
        level = maxBrightness;

    if(i >= 0 && i < NUM_DIMMER_PINS)
    {
        dimmerLevels[i] = level;
    }
    else
    {
        printf("dimmer number out of range");
    }
}

void Dimmer::setMaxBrightness(uint8_t level) {
    maxBrightness = level;
    for(int i = 0; i < NUM_DIMMER_PINS; ++i)
    {
        if(dimmerLevels[i] > maxBrightness)
            dimmerLevels[i] = maxBrightness;
    }
}

void Dimmer::setMinBrightness(uint8_t level) {
    minBrightness = level;
    for(int i = 0; i < NUM_DIMMER_PINS; ++i)
    {
        if(dimmerLevels[i] < minBrightness)
            dimmerLevels[i] = 0;
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


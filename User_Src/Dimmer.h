#pragma once

#include <cstdint>
#include "Pin.h"
#include "gpio.h"

class Dimmer {
private:
    static const int NUM_DIMMER_PINS = 4;
    static const Pin pins[NUM_DIMMER_PINS];
    static uint8_t dimmerLevels[NUM_DIMMER_PINS];
    static uint16_t currentTick;

    static void setPinOn(const Pin& pin, bool on);


public:

    static void init();
    static void setDimmerLevel(int i, uint8_t level);




    /////////////////////////////////////////////////
    //PRIVATE METHODS BELOW. DO NOT USE OUTSIDE CLASS
    /////////////////////////////////////////////////

    /**Notify this dimmer that a zero crossing has occurred */
    static void notifyZeroCrossing();

    /** Notify this dimmer about a timer tick */
    static void notifyTimerTick();


};

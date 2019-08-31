#pragma once

#include <cstdint>
#include "Pin.h"
#include "gpio.h"

class Dimmer {
public:
    static const int NUM_DIMMERS = 12;

private:

    static const Pin pins[NUM_DIMMERS];

    /**Contains the tick at which the i'th dimmer pulse started or -1 if it hasnt started  */
    static int16_t lastPulseTime[NUM_DIMMERS];

    static uint8_t dimmerLevels[NUM_DIMMERS];
    static uint16_t currentTick;

    static uint8_t minBrightness;
    static uint8_t maxBrightness;

    static bool timerLock;


    static void setPinOn(const Pin& pin, bool on);


public:

    static void init();
    static void setDimmerLevel(int i, uint8_t level);
    static void setMinBrightness(uint8_t level);
    static void setMaxBrightness(uint8_t level);





    /////////////////////////////////////////////////
    //PRIVATE METHODS BELOW. DO NOT USE OUTSIDE CLASS
    /////////////////////////////////////////////////

    /**Notify this dimmer that a zero crossing has occurred */
    static void notifyZeroCrossing();

    /** Notify this dimmer about a timer tick */
    static void notifyTimerTick();


};

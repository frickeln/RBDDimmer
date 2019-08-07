#pragma once

#include <cstdint>
#include "Pin.h"

class Dimmer {
private:
    static const int NUM_DIMMER_PINS = 4;
    const Pin pins[NUM_DIMMER_PINS];
    uint8_t dimmerLevels[NUM_DIMMER_PINS];
    uint16_t currentTick = 0;


    void setPinOn(const Pin& pin, bool on);

public:
    Dimmer(const Pin& p1,  const Pin& p2, const Pin& pin3, const Pin&  pin4);

    /**Notify this dimmer that a zero crossing has occurred */
    void notifyZeroCrossing();

    /** Notify this dimmer about a timer tick */
    void notifyTimerTick();

    void setDimmerLevel(int i, uint8_t level);
};

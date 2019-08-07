//
// Created by arne on 05.08.19.
//

#include <cstdio>
#include "Dimmer.h"


Dimmer::Dimmer(const Pin &p1, const Pin &p2, const Pin &p3, const Pin &p4) : pins{p1, p2, p3, p4}, dimmerLevels{0, 0, 0, 0}
{

}

void Dimmer::notifyZeroCrossing() {
    //TODO zero crossing needs higher prio than timer (or lower? not sure yet)
    //TODO not sure if that could work? depends on implementation in timer tick

    //turn all lamps off on zero crossing
    for(int i = 0; i < NUM_DIMMER_PINS; ++i) {
        //turn all lamps off for at least one tick unless dimmer level is at 100%
        if(dimmerLevels[i] != 255)
            setPinOn(pins[i], false);
    }
    currentTick = 0;
}

void Dimmer::notifyTimerTick() {

    ++currentTick; //TODO fix race condition

    for(int i = 0; i < NUM_DIMMER_PINS; ++i) {
        if(currentTick >= (255 - dimmerLevels[i]))
            setPinOn(pins[i], true);
    }

}



void Dimmer::setPinOn(const Pin &pin, bool on) {

}

void Dimmer::setDimmerLevel(int i, uint8_t level) {
    if(i > 0 && i < NUM_DIMMER_PINS)
    {
        dimmerLevels[i] = level;
    }
    else
    {
        printf("dimmer number out of range");
    }
}

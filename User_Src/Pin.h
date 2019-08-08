#pragma once

#include "gpio.h"
struct Pin
{
    GPIO_TypeDef* port;
    int pin;
};

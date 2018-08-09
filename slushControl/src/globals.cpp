#include "globals.h"

void shiftRegisterWrite(uint8_t bit, bool state) {
    if (!state) //inverted relay logic
        shiftRegisterState |= (1 << bit);
    else
        shiftRegisterState &= ~(1 << bit);
}

uint8_t shiftRegisterState = 255;
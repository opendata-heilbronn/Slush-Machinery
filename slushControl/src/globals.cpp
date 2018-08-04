#include "globals.h"

void shiftRegisterWrite(uint8_t bit, bool state) {
    if (!state) //inverted relay logic
        shiftRegisterState |= (1 << bit);
    else
        shiftRegisterState &= ~(1 << bit);

    digitalWrite(PIN_SR_LATCH, LOW);
    shiftOut(PIN_SR_DATA, PIN_SR_CLK, MSBFIRST, shiftRegisterState);
    digitalWrite(PIN_SR_LATCH, HIGH);
}

uint8_t shiftRegisterState = 255;
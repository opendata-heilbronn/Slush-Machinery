#pragma once

#include "config.h"
#include <Arduino.h>

uint8_t shiftRegisterState = 0;

void shiftRegisterWrite(uint8_t bit, bool state) {
    if (state)
        shiftRegisterState |= (1 << bit);
    else
        shiftRegisterState &= ~(1 << bit);

    digitalWrite(PIN_SR_LATCH, LOW);
    shiftOut(PIN_SR_DATA, PIN_SR_CLK, MSBFIRST, shiftRegisterState);
    digitalWrite(PIN_SR_LATCH, HIGH);
}
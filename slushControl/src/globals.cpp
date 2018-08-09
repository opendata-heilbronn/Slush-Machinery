#include "globals.h"

void shiftRegisterWrite(uint8_t bit, bool state) {
    if (!state) //inverted relay logic
        shiftRegisterState |= (1 << bit);
    else
        shiftRegisterState &= ~(1 << bit);

    Serial.println(shiftRegisterState, 2);

    noInterrupts();
    // shitty workaround because of some interference
    for(uint8_t i = 0; i < 10; i++) {
        digitalWrite(PIN_SR_LATCH, LOW);
        shiftOut(PIN_SR_DATA, PIN_SR_CLK, MSBFIRST, shiftRegisterState);
        digitalWrite(PIN_SR_LATCH, HIGH);
        delay(3);
    }
    
    interrupts();
}

uint8_t shiftRegisterState = 255;
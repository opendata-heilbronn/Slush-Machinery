#include "globals.h"
#include <Arduino.h>
#include <slushMachine.h>
#include "display.h"
#include "webInterface.h"
#include "ota.h"

SlushMachine slushMachineLeft = SlushMachine(BIT_SML_MOTOR, BIT_SML_VALVE, PIN_SML_ENCODER, PIN_SML_NTC);
SlushMachine slushMachineRight = SlushMachine(BIT_SMR_MOTOR, BIT_SMR_VALVE, PIN_SMR_ENCODER, PIN_SMR_NTC);

SlushMachine slushMachines[NUM_SLUSH_MACHINES] = {slushMachineLeft, slushMachineRight};
uint8_t slushMachinesLength = sizeof(slushMachines)/sizeof(slushMachines[0]);

void ISR_slushLeft() { slushMachineLeft.isr(); }
void ISR_slushRight() { slushMachineRight.isr(); }

void initShiftRegister() {
    pinMode(PIN_SR_CLK, OUTPUT);
    pinMode(PIN_SR_DATA, OUTPUT);
    pinMode(PIN_SR_LATCH, OUTPUT);
}

void setup() {
    Serial.begin(115200);
    Serial.print("\nSlush Machinery initializing... ");
    
    initShiftRegister();
    initDisplay();
    initOTA();

    slushMachineLeft.init();
    slushMachineRight.init();
    attachInterrupt(PIN_SML_ENCODER, ISR_slushLeft, FALLING);
    attachInterrupt(PIN_SMR_ENCODER, ISR_slushRight, FALLING);

    initWebInterface(slushMachines);

    Serial.println("done.");
}

void loop() {
    loopDisplay(slushMachines);
    loopWebInterface();
    loopOTA();
    slushMachineLeft.loop();
    slushMachineRight.loop();
}
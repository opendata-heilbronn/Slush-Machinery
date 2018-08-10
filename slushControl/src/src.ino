#include "display.h"
#include "globals.h"
#include "ota.h"
#include "webInterface.h"
#include <Arduino.h>
#include <slushMachine.h>

#define SHIFTREGISTER_WRITE_INTERVAL 1

SlushMachine slushMachineLeft = SlushMachine(BIT_SML_MOTOR, BIT_SML_VALVE, PIN_SML_ENCODER, PIN_SML_NTC);
SlushMachine slushMachineRight = SlushMachine(BIT_SMR_MOTOR, BIT_SMR_VALVE, PIN_SMR_ENCODER, PIN_SMR_NTC);

SlushMachine *slushMachines[NUM_SLUSH_MACHINES] = {&slushMachineLeft, &slushMachineRight};
uint8_t slushMachinesLength = sizeof(slushMachines) / sizeof(slushMachines[0]);

void ISR_slushLeft() { slushMachineLeft.isr(); }
void ISR_slushRight() { slushMachineRight.isr(); }

void initShiftRegister() {
    pinMode(PIN_SR_CLK, OUTPUT);
    pinMode(PIN_SR_DATA, OUTPUT);
    pinMode(PIN_SR_LATCH, OUTPUT);
}

void writeShiftRegister() {
    noInterrupts();
    digitalWrite(PIN_SR_LATCH, LOW);
    shiftOut(PIN_SR_DATA, PIN_SR_CLK, MSBFIRST, shiftRegisterState);
    digitalWrite(PIN_SR_LATCH, HIGH);
    interrupts();
}

void setup() {
    Serial.begin(115200);
    Serial.print("\nSlush Machinery initializing... ");

    initShiftRegister();
    initDisplay();
    initOTA();

    // turn fan on on startup
    shiftRegisterWrite(BIT_FAN, 1);

    slushMachineLeft.init();
    slushMachineRight.init();
    attachInterrupt(PIN_SML_ENCODER, ISR_slushLeft, FALLING);
    attachInterrupt(PIN_SMR_ENCODER, ISR_slushRight, FALLING);

    initWebInterface(slushMachines);

    Serial.println("done.");
}

uint32_t lastShiftRegisterWrite = 0, lastDisplayInit = 0, lastDebugOutput = 0;

void loop() {
    loopDisplay(slushMachines);
    loopWebInterface();
    loopOTA();

    slushMachineLeft.loop();
    slushMachineRight.loop();

    // simply write to shift register very often to mitigate interference
    if (millis() - lastShiftRegisterWrite >= SHIFTREGISTER_WRITE_INTERVAL) {
        lastShiftRegisterWrite = millis();
        writeShiftRegister();
    }

    // reinitialize display once in a while
    if (millis() - lastDisplayInit >= 60000) {
        lastDisplayInit = millis();
        initDisplay();
    }

    if (millis() - lastDebugOutput >= 100) {
        lastDebugOutput = millis();
        Serial.printf("%f %i %i %f %i %i", 
            slushMachineLeft.getTemperature(), 
            analogRead(PIN_SML_NTC),
            slushMachineLeft.getMotorRevsPerMin() / 60, 
            slushMachineRight.getTemperature(), 
            analogRead(PIN_SMR_NTC),
            slushMachineRight.getMotorRevsPerMin() / 60,
        );
    }
}
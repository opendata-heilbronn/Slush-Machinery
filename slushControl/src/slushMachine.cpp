#include <Arduino.h>
#include <slushMachine.h>

SlushMachine::SlushMachine(uint8_t motorPin, uint8_t motorSpeedPin, uint8_t valvePin, uint8_t ntcPin) {
    this->motorPin = motorPin;
    this->motorSpeedPin = motorSpeedPin;
    this->valvePin = valvePin;
    this->ntcPin = ntcPin;
}

void SlushMachine::isr() { motorRevs++; }

void SlushMachine::checkMotor() {
    revsPerSec = motorRevs * (1000 / MOTOR_CHECK_INTERVAL);
    avgRevs = avgRevs * (1 - MOTOR_REVS_AVG_FACTOR) + (float)revsPerSec * MOTOR_REVS_AVG_FACTOR;
    
    //TODO: motor stall check

    //TODO: consistency control
}

void SlushMachine::init() {}

void SlushMachine::loop() {
    uint32_t now = millis();

    if (lastMotorCheck + MOTOR_CHECK_INTERVAL > now) {
        lastMotorCheck = now;
        checkMotor();
    }
}
#include <Arduino.h>
#include <globals.h>
#include <slushMachine.h>

SlushMachine::SlushMachine(uint8_t motorSrBit, uint8_t valveSrBit, uint8_t motorSpeedPin, uint8_t ntcPin) {
    this->motorSrBit = motorSrBit;
    this->valveSrBit = valveSrBit;
    this->motorSpeedPin = motorSpeedPin;
    this->ntcPin = ntcPin;
}

void SlushMachine::isr() { motorRevs++; }

void SlushMachine::checkMotor() {
    revsPerSec = motorRevs * (1000 / MOTOR_CHECK_INTERVAL);
    avgRevs = avgRevs * (1 - MOTOR_REVS_AVG_FACTOR) + (float)revsPerSec * MOTOR_REVS_AVG_FACTOR;

    Serial.print("      ");
    Serial.println((uint16_t)(avgRevs * 60));

    // TODO: motor stall check

    // TODO: consistency control

    motorRevs = 0;
}

void SlushMachine::setMotorState(bool state) {
    motorState = state;
    shiftRegisterWrite(motorSrBit, state);
}

void SlushMachine::setValveState(bool state) {
    valveState = state;
    shiftRegisterWrite(valveSrBit, state);
}

// TODO: check if non-linearity of ESP32 ADC is a problem (https://www.esp32.com/viewtopic.php?f=19&t=2881&start=10#p13739)
// convert temperature of NTC using the simplified b parameter Steinhart equation
// (https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation)
float SlushMachine::getTemperature() {
    uint16_t adc = analogRead(ntcPin);
    float ntcResistance = NTC_DIVIDER_RESISTANCE / ((4095 / (float)adc) - 1);

    //                   1 / ((               ln(R/Ro)                       * 1/B              ) +    (1/To) )
    float temperature =
        (1 / (((log(ntcResistance / NTC_NOMINAL_RESISTANCE)) / NTC_B_COEFFICIENT) + (1.0 / (NTC_NOMINAL_TEMPERATURE + 273.15)))) - 273.15;
    return temperature;
}

uint16_t SlushMachine::getMotorRevsPerMin() {
    //WHY THE F IS THIS ALWAYS ZERO ?! TODO: INVESTIGATE
    Serial.println((uint16_t)(avgRevs * 60));
    return (uint16_t)(avgRevs * 60);
}

bool SlushMachine::getMotorState() { return motorState; }

bool SlushMachine::getValveState() { return valveState; }

void SlushMachine::init() {
    pinMode(motorSpeedPin, INPUT);
    setMotorState(0);
    setValveState(0);
}

void SlushMachine::loop() {
    uint32_t now = millis();

    if (lastMotorCheck + MOTOR_CHECK_INTERVAL < now) {
        lastMotorCheck = now;
        checkMotor();
    }
}
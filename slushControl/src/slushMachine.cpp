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

void SlushMachine::setCooling(bool state) { coolingEnabled = state; }

// convert temperature of NTC using the simplified b parameter Steinhart equation
// (https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation)
float SlushMachine::readTemperature() {
    uint16_t rawAdc = analogRead(ntcPin);

    float ntcResistance = NTC_DIVIDER_RESISTANCE / ((ADC_FULLSCALE_VALUE / ((float)rawAdc + ADC_OFFSET)) - 1);

    // 1 / ((ln(R/Ro) * 1/B) + (1/To))
    float temperature =
        (1 / (((log(ntcResistance / NTC_NOMINAL_RESISTANCE)) / NTC_B_COEFFICIENT) + (1.0 / (NTC_NOMINAL_TEMPERATURE + 273.15)))) - 273.15;
    return temperature;
}

float SlushMachine::getTemperature() { return avgTemp; }

uint16_t SlushMachine::getMotorRevsPerMin() { return (uint16_t)(avgRevs * 60); }

bool SlushMachine::getMotorState() { return motorState; }

bool SlushMachine::getValveState() { return valveState; }

bool SlushMachine::getCoolingState() { return coolingEnabled; }

void SlushMachine::setTemperature(float temp) { temperatureSetPoint = temp; }

float SlushMachine::getSetTemperature() { return temperatureSetPoint; }

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

    if (lastTempRead + TEMP_READ_INTERVAL < now) {
        lastTempRead = now;
        float temp = readTemperature();
        avgTemp = avgTemp * (1 - TEMP_AVG_FACTOR) + temp * TEMP_AVG_FACTOR;
    }

    // simple bang-bang temperature control
    if (lastTempControl + temperatureControlInterval < now && coolingEnabled) {
        lastTempControl = now;
        setValveState(avgTemp > temperatureSetPoint);
    }

    // limit cooling duty cycle
    if(getValveState() && lastTempControl + ((temperatureControlInterval * maxCoolingDuty) / 100) < now) {
        setValveState(false);
    }
}
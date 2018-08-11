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

void SlushMachine::temperatureController() {
    double inputTemp = avgTemp;
    uint32_t now = millis();
    double timeDiff = (now - lastPIDTime);

    double error = -(temperatureSetPoint - inputTemp);
    errSum += error;
    errSum = constrain(errSum, OUT_MIN, OUT_MAX);
    double dErr = (error - lastError);

    double output = kp * error + ki * errSum + kd * dErr;
    output = constrain(output, OUT_MIN, OUT_MAX);
    lastError = error;

    // scale output to relais window time (probably)

    coolingOnTime = output;

    Serial.printf("err: %6.2f - errSum: %6.2f - output: %6.2f - coolTime: %6i\n", error, errSum, output, coolingOnTime);
}

uint16_t SlushMachine::getMotorRevsPerMin() { return (uint16_t)(avgRevs * 60); }

bool SlushMachine::getMotorState() { return motorState; }

bool SlushMachine::getValveState() { return valveState; }

bool SlushMachine::getCoolingState() { return coolingEnabled; }

void SlushMachine::setTemperature(float temp) { temperatureSetPoint = temp; }

float SlushMachine::getSetTemperature() { return temperatureSetPoint; }

double SlushMachine::getPID(uint8_t id) {
    switch (id) {
    case 0:
        return kp;
    case 1:
        return ki;
    case 2:
        return kd;
    default:
        return 0;
    }
}

void SlushMachine::setPids(double pkp, double pki, double pkd) {
    kp = pkp;
    ki = pki;
    kd = pkd;
}

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

    if (lastTempControl + TEMP_CONTROL_WINDOW < now && coolingEnabled) {
        lastTempControl = now;
        temperatureController();
        if (coolingOnTime > 0) {
            setValveState(true);
        }
    }
    // turn valve back off
    if ((now - lastTempControl) > coolingOnTime && valveState) {
        setValveState(false);
    }
}
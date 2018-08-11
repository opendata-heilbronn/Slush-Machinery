#pragma once
#include <Arduino.h>

#define MOTOR_CHECK_INTERVAL    100
#define MOTOR_REVS_AVG_FACTOR   0.1

#define TEMP_READ_INTERVAL      50
#define TEMP_AVG_FACTOR         0.05

#define TEMP_INIT_SETPOINT      -5
// time window for relais operation
#define TEMP_CONTROL_WINDOW     10000

class SlushMachine {
    public:
        SlushMachine(uint8_t motorSrBit, uint8_t valveSrBit, uint8_t motorSpeedPin, uint8_t ntcPin);
        void init();
        void loop();
        void IRAM_ATTR isr();
        float getTemperature();
        uint16_t getMotorRevsPerMin();
        bool getMotorState();
        bool getValveState();
        bool getCoolingState();
        void setMotorState(bool state);
        void setValveState(bool state);
        void setCooling(bool state);
        void setTemperature(float temp);
        float getSetTemperature();

    private:
        void checkMotor();
        float readTemperature();
        void temperatureController();
        
        uint8_t motorSrBit, valveSrBit, motorSpeedPin, ntcPin;
        volatile uint16_t motorRevs = 0;
        uint16_t revsPerSec;
        uint32_t lastMotorCheck = 0, lastTempRead = 0, lastTempControl = 0;
        bool motorState = 0, valveState = 0;
        float avgRevs, avgTemp;
        bool coolingEnabled = false;
        float temperatureSetPoint = TEMP_INIT_SETPOINT;

};
#include <Arduino.h>

#define MOTOR_CHECK_INTERVAL    10
#define MOTOR_REVS_AVG_FACTOR   0.1

class SlushMachine {
    public:
        SlushMachine(uint8_t motorSrBit, uint8_t valveSrBit, uint8_t motorSpeedPin, uint8_t ntcPin);
        void init();
        void loop();
        void IRAM_ATTR isr();
        float getTemperature();
    private:
        void checkMotor();
        void setMotorState(bool state);
        void setValveState(bool state);
        
        uint8_t motorSrBit, valveSrBit, motorSpeedPin, ntcPin;
        volatile uint16_t motorRevs = 0;
        uint16_t revsPerSec;
        uint32_t lastMotorCheck = 0;
        float avgRevs = 0;
};
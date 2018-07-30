#include <Arduino.h>

#define MOTOR_CHECK_INTERVAL    10
#define MOTOR_REVS_AVG_FACTOR   0.1

class SlushMachine {
    public:
        SlushMachine(uint8_t motorPin, uint8_t motorSpeedPin, uint8_t valvePin, uint8_t ntcPin);
        void init();
        void loop();
        void IRAM_ATTR isr();
    private:
        void checkMotor();
        
        uint8_t motorPin, motorSpeedPin, valvePin, ntcPin;
        uint16_t motorRevs = 0, revsPerSec;
        uint32_t lastMotorCheck = 0;
        float avgRevs = 0;
};
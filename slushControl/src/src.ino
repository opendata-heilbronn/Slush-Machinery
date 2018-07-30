#include "config.h"
#include <Arduino.h>
#include <slushMachine.h>
#include "display.h"

SlushMachine slushMachineLeft = SlushMachine(0, 1, 2, 3);
SlushMachine slushMachineRight = SlushMachine(4, 5, 6, 7);

SlushMachine slushMachines[] = {slushMachineLeft, slushMachineRight};
uint8_t slushMachinesLength = sizeof(slushMachines)/sizeof(slushMachines[0]);

void ISR_slushLeft() { slushMachineLeft.isr(); }
void ISR_slushRight() { slushMachineRight.isr(); }

void setup() {
    Serial.begin(115200);
    Serial.println("\nSlush Machinery initializing...");
    
    initDisplay();

    slushMachineLeft.init();
    slushMachineRight.init();
    attachInterrupt(1, ISR_slushLeft, FALLING);
    attachInterrupt(5, ISR_slushRight, FALLING);
}

void loop() {
    slushMachineLeft.loop();
    slushMachineRight.loop();
}
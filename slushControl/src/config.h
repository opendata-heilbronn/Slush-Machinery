#pragma once

// Shift register pins
#define PIN_SR_CLK      4
#define PIN_SR_DATA     15
#define PIN_SR_LATCH    2

// Left slush machine defines, BIT_ are pins on the shift register, PIN_ are GPIOs on the ESP32
#define BIT_SML_MOTOR   0
#define BIT_SML_VALVE   0
#define PIN_SML_NTC     34
#define PIN_SML_ENCODER 39

// Right slush machine defines
#define BIT_SMR_MOTOR   0
#define BIT_SMR_VALVE   0
#define PIN_SMR_NTC     35
#define PIN_SMR_ENCODER 36

#define PIN_RADIATOR_NTC    32

// Thermistor config
// beta coefficient of the thermistor (todo: tune)
#define NTC_B_COEFFICIENT       3950
// nominal thermistor temperature
#define NTC_NOMINAL_TEMPERATURE 25
// nominal thermistor resistance
#define NTC_NOMINAL_RESISTANCE  1000
// voltage divider resistor
#define NTC_DIVIDER_RESISTANCE  1000
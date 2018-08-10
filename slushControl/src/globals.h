#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include "config.h"
#include <Arduino.h>

#define NUM_SLUSH_MACHINES 2

//ADC scaling (calculate which adc value a full 3.3V would give if the range was there)
#define ADC_FULLSCALE_VALUE     7083
//ADC offset in ADC steps
#define ADC_OFFSET              221

// Shift register pins
#define PIN_SR_CLK      4
#define PIN_SR_DATA     15
#define PIN_SR_LATCH    2

// Left slush machine defines, BIT_ are pins on the shift register, PIN_ are GPIOs on the ESP32
#define BIT_SML_MOTOR   6
#define BIT_SML_VALVE   7
#define PIN_SML_NTC     35
#define PIN_SML_ENCODER 36

// Right slush machine defines
#define BIT_SMR_MOTOR   4
#define BIT_SMR_VALVE   5
#define PIN_SMR_NTC     32
#define PIN_SMR_ENCODER 39

#define PIN_RADIATOR_NTC    32
#define BIT_COMPRESSOR  0
#define BIT_FAN         3

// Thermistor config
// beta coefficient of the thermistor (todo: tune)
#define NTC_B_COEFFICIENT       3950 
// nominal thermistor temperature
#define NTC_NOMINAL_TEMPERATURE 25
// nominal thermistor resistance
#define NTC_NOMINAL_RESISTANCE  1000
// voltage divider resistor
#define NTC_DIVIDER_RESISTANCE  4700

extern uint8_t shiftRegisterState;

extern void shiftRegisterWrite(uint8_t bit, bool state);

#endif
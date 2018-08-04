#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include "config.h"
#include <Arduino.h>

extern uint8_t shiftRegisterState;

extern void shiftRegisterWrite(uint8_t bit, bool state);

#endif
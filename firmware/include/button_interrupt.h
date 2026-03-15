#pragma once
#include <Arduino.h>

extern volatile bool doorbellInterrupted;

void IRAM_ATTR handleButtonInterrupt();
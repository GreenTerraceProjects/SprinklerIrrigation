#ifndef _IRRIGATION_HPP_
#define _IRRIGATION_HPP_

#include <RTClib.h>

enum IrrigationMode {
  ALWAYS_OFF,
  ALWAYS_ON,
  CYCLIC,
  CUSTOM,
  REMOTE_CONTROL_MCU
};

extern enum IrrigationMode irrigationMode;

void changeIrrigationMode(enum IrrigationMode mode);
void irrigate(enum IrrigationMode mode, RTC_Micros* rtc);
void setupDefaultIrrigationMode();

#endif

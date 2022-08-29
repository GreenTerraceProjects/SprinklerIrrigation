#include <EEPROM.h>
#include "irrigation.hpp"
#include "../devices/pump/pump.hpp"
#include "../devices/eeprom/eeprom-addresses.hpp"

enum IrrigationMode irrigationMode = ALWAYS_OFF;

void changeIrrigationMode(enum IrrigationMode mode) {
  irrigationMode = mode;
}

void irrigateWithCyclicMode(RTC_Micros* rtc) {
  uint32_t eepromValue;
  int eepromAddress = EEPROM_CYCLIC_MODE_SETTINGS_ADDRESS;

  EEPROM.get(eepromAddress, eepromValue);
  TimeSpan fullCycleDuration(eepromValue);

  eepromAddress += sizeof(uint32_t);
  EEPROM.get(eepromAddress, eepromValue);
  TimeSpan dutyCycleDuration(eepromValue);

  eepromAddress += sizeof(uint32_t);
  EEPROM.get(eepromAddress, eepromValue);
  TimeSpan startTimeAsTimeSpan(eepromValue);

  eepromAddress += sizeof(uint32_t);
  EEPROM.get(eepromAddress, eepromValue);
  TimeSpan stopTimeAsTimeSpan(eepromValue);

  DateTime now = rtc->now();  
  DateTime beginOfTheDay(now.year(), now.month(), now.day(), 0, 0, 0);
  TimeSpan oneDayDuration(1, 0, 0, 0);

  DateTime startTime = beginOfTheDay + startTimeAsTimeSpan;
  DateTime stopTime  = beginOfTheDay + stopTimeAsTimeSpan;

  bool crossMidNight = startTimeAsTimeSpan.totalseconds() 
                        >= stopTimeAsTimeSpan.totalseconds();
  if (crossMidNight) {
    if (now < stopTime) {
      startTime = startTime - oneDayDuration;
    } else {
      stopTime = stopTime + oneDayDuration;
    }
  }

  if (now >= startTime && now < stopTime) {
    uint16_t cycleElapsedSeconds = (now - startTime).totalseconds() % fullCycleDuration.totalseconds();
    if (cycleElapsedSeconds < dutyCycleDuration.totalseconds()) {
      startPump();
    } else {
      stopPump();
    }
  } else {
    stopPump();
  }
}

DateTime irrgateWithCustomMode(RTC_Micros* rtc) {
  DateTime now = rtc->now();

  int eepromAddress = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS;
  uint32_t startTimeAsSeconds;
  uint32_t duration;
  bool notFound = true;
  for (int i = 0; i < NUM_CUSTOM_MODE_SETTINGS; ++i) {
    EEPROM.get(eepromAddress, startTimeAsSeconds);
    eepromAddress += sizeof(uint32_t);
    EEPROM.get(eepromAddress, duration);
    eepromAddress += sizeof(uint32_t);

    if (duration <= 0) continue;

    TimeSpan startTimeAsTimeSpan(startTimeAsSeconds);
    DateTime startTime(now.year(), now.month(), now.day(), 
    startTimeAsTimeSpan.hours(), startTimeAsTimeSpan.minutes(), startTimeAsTimeSpan.seconds());
    DateTime stopTime = startTime + TimeSpan(duration);

    if (now >= startTime && now < stopTime ||
        now >= startTime - TimeSpan(1, 0, 0, 0)
          && now < stopTime- TimeSpan(1, 0, 0, 0)) {
      notFound = false;
      startPump();
      break;
    }
  }

  if (notFound) {
      stopPump();
  }
}

void irrigate(enum IrrigationMode mode, RTC_Micros* rtc) {
  switch (mode) {
    case ALWAYS_OFF:
      stopPump();
      break;
    case ALWAYS_ON:
      startPump();
      break;
    case CYCLIC:
      irrigateWithCyclicMode(rtc);
      break;
    case CUSTOM:
      irrgateWithCustomMode(rtc);
      break;
    default:
      break;
  }
}

void setupDefaultIrrigationMode() {
  uint8_t defaultIrrigationMode = 0;
  EEPROM.get(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, defaultIrrigationMode);
  irrigationMode = (IrrigationMode)defaultIrrigationMode;
}

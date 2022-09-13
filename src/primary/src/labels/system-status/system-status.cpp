#include "system-status.hpp"
#include <RTClib.h>
#include "../../devices/rtc/rtc.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/lcd/lcd-shadow.hpp"
#include "../../devices/pump/pump.hpp"
#include "../../irrigation/irrigation.hpp"
#include "../timespan-label/timespan-label.hpp"
#include "../datetime-label/datetime-label.hpp"

DateTime lastDisplay(2022, 4, 30, 23, 59, 59);

int8_t displayStatusCounter = 0;
const int8_t DISPLAY_STATUS_COUNTER_MAX_VALUE = 5;

bool displayImmediately = false;
enum DisplayStatusType currentDisplayStatus = DisplayStatusType::CurrentDateTime;

void setDisplayStatus(enum DisplayStatusType type) {
  displayImmediately = true;
  currentDisplayStatus = type;
}

void jumpToNextDisplayStatusType() {
  switch (currentDisplayStatus) {
    case DeviceTitle:
      currentDisplayStatus = CurrentDateTime;
      break;
    case CurrentDateTime:
      currentDisplayStatus = CurrentIrrigationMode;
      break;
    case CurrentIrrigationMode:
      currentDisplayStatus = CurrentPumpStatus;
      break;
    case CurrentPumpStatus:
      currentDisplayStatus = DeviceTitle;
      break;
    default:
      currentDisplayStatus = DeviceTitle;
      break;
  }
}

void printStatus() {
  DateTime now = rtc.now();

  if (currentDisplayStatus == DeviceTitle ||
      currentDisplayStatus == CurrentIrrigationMode) {
    lcdShadow.clear();
    lcd.noCursor();
  }
  lcd.noBlink();

  if (currentDisplayStatus == DeviceTitle) {
    lcdShadow.print(F("Sprinkler"));
    lcdShadow.setCursor(0, 1);
    lcdShadow.print(F("Irrigation"));
    lcdShadow.commit();
  } else if (currentDisplayStatus == CurrentIrrigationMode) {
    lcdShadow.print(F("Irrigation Mode:"));
    lcdShadow.setCursor(0, 1);
    switch (irrigationMode) {
      case ALWAYS_OFF:
        lcdShadow.print(F("[A] - Always Off"));
        break;
      case ALWAYS_ON:
        lcdShadow.print(F("[B] - Always On"));
        break;
      case CYCLIC:
        lcdShadow.print(F("[C] - Cyclic"));
        break;
      case CUSTOM:
        lcdShadow.print(F("[D] - Custom"));
        break;
      default:
        break;
    }
    lcdShadow.commit();
  } else if (currentDisplayStatus == CurrentPumpStatus) {
    if (isPumpOn) {
      TimeSpan workingDuration = now - pumpStartedAt;
      displayTimeSpan(true, &workingDuration, PSTR("Pump: ON"));
    } else {
      TimeSpan stoppingDuration = now - pumpStoppedAt;
      displayTimeSpan(true, &stoppingDuration, PSTR("Pump: OFF"));
    }
  } else {
    displayDateTime(&now);
  }

  lcd.setCursor(LCD_COLS - 1, 1);
  lcd.blink();
}

void displayStatus() {
  if (lastDisplay == rtc.now() && !displayImmediately) {
    return;
  }
  lastDisplay = rtc.now();

  if (displayImmediately) {
    displayStatusCounter = 0;
    displayImmediately = false;
  } else {
    if (displayStatusCounter > DISPLAY_STATUS_COUNTER_MAX_VALUE) {
      displayStatusCounter = 0;
      jumpToNextDisplayStatusType();
    } else {
      ++displayStatusCounter;
    }
  }

  printStatus();
}

/*
A: Always Off
B: Always On
C: Cyclic Mode: Pump will run in duty cycle of a full cycle, from start time to stop time.
D: Custom Mode: There are 24 slots for setting starting time and running duration of pump.
*/

#include <EEPROM.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "src/modules/devices/pins/pins.hpp"
#include "src/modules/devices/eeprom/addresses.hpp"
#include "src/modules/devices/rtc/rtc.hpp"
#include "src/modules/devices/lcd/lcd.hpp"
#include "src/modules/devices/keypad/keypad.hpp"
#include "src/modules/devices/buzzer/buzzer.hpp"
#include "src/modules/devices/relays/relays.hpp"
#include "src/modules/devices/pump/pump.hpp"
#include "src/modules/inputs/menu/menu.hpp"

#include "src/modules/labels/time-span-label/time-span-label.hpp"
#include "src/modules/labels/date-time-label/date-time-label.hpp"

#include "src/modules/settings/default-irrigation-mode/default-irrigation-mode.hpp"
#include "src/modules/settings/cyclic-mode-settings/cyclic-mode-settings.hpp"
#include "src/modules/settings/custom-mode-entries/custom-mode-entries.hpp"
#include "src/modules/settings/pump-settings/pump-settings.hpp"
#include "src/modules/settings/rtc-adjustment/rtc-adjustment.hpp"
#include "src/modules/settings/eeprom-cleaning/eeprom-cleaning.hpp"

#include "src/modules/irrigation/irrigation.hpp"

enum DisplayStatusType {
  DeviceTitle,
  CurrentDateTime,
  CurrentIrrigationMode,
  CurrentPumpStatus
};

void setup() {
  wdt_enable(WDTO_8S);
  
  setupRTC();
  setupLCD();
  setupKeypad();
  setupRelays();
  setupDefaults();

  beep();
  wdt_reset();
}

void loop() {
  wdt_reset();
  keepInternalRTCUpToDate();

  chooseFunction();
  irrigate(irrigationMode, &rtc);

  displayStatus();
}

void setupRelays() {
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(VALVE_RELAY_PIN, OUTPUT);
  turnOffPumpRelay();
  turnOffValveRelay();
}

void setupDefaults() {
  uint8_t defaultIrrigationMode = 0;
  EEPROM.get(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, defaultIrrigationMode);
  irrigationMode = (IrrigationMode)defaultIrrigationMode;

  pumpStartedAt = pumpStoppedAt = rtc.now();
}

/*
void setupInterrupts() {
  pinMode(D1307_SQW_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(D1307_SQW_INTERRUPT_PIN), D1307SwqIsr, CHANGE);
}
*/

void reset() {
  lcd.clear();
  lcd.print(F("Restarting ..."));
  lcd.blink();
  
  wdt_disable();
  wdt_enable(WDTO_2S);
  wdt_reset();
  for (;;) {}
}

DateTime lastDisplay(2022, 4, 30, 23, 59, 59);
int8_t displayStatusCounter = 0;
const int8_t DISPLAY_STATUS_COUNTER_MAX_VALUE = 5;
bool displayImmediately = false;
enum DisplayStatusType currentDisplayStatus = CurrentDateTime;

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

void displayStatus() {
  if (!(lastDisplay != rtc.now() || displayImmediately)) {
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
    } {
      ++displayStatusCounter;
    }
  }

  lcd.clear();
  lcd.lineWrap();
  lcd.noCursor();

  DateTime now = rtc.now();

  if (currentDisplayStatus == DeviceTitle) {
    lcd.print(F("Sprinkler"));
    lcd.setCursor(0, 1);
    lcd.print(F("Irrigation"));
  } else if (currentDisplayStatus == CurrentIrrigationMode) {
    lcd.print(F("Mode:"));
    switch (irrigationMode) {
      case ALWAYS_OFF:
        lcd.print(F("[A] - Always Off"));
        break;
      case ALWAYS_ON:
        lcd.print(F("[B] - Always On"));
        break;
      case CYCLIC:
        lcd.print(F("[C] - Cyclic"));
        break;
      case CUSTOM:
        lcd.print(F("[D] - Custom"));
        break;
      default:
        break;
    } 
  } else if (currentDisplayStatus == CurrentPumpStatus) {
    if (isPumpOn) {
      displayTimeSpan(now - pumpStartedAt, true, PSTR("Pump: ON"));
    } else {
      displayTimeSpan(now - pumpStoppedAt, true, PSTR("Pump: OFF"));
    }
  } else {
    displayDateTime(now);
  }

  lcd.setCursor(15, 1);
  lcd.blink();
}

void chooseFunction() {
  char key = keypad.getKey();

  if (!key) {
    return;
  }

  switch (key) {
    case 'A':
      changeIrrigationMode(ALWAYS_OFF);
      break;
    case 'B':
      changeIrrigationMode(ALWAYS_ON);
      break;
    case 'C':
      changeIrrigationMode(CYCLIC);
      break;
    case 'D':
      changeIrrigationMode(CUSTOM);
      break;
    case '1':
      setDisplayStatus(CurrentDateTime);
      break;
    case '2':
      setDisplayStatus(CurrentIrrigationMode);
      break;
    case '3':
      setDisplayStatus(CurrentPumpStatus);
      break;
    case '*':
      highBeep();
      changeSettings();
      break;
    default:
      break;
  }
}

void changeIrrigationMode(enum IrrigationMode mode) {
  irrigationMode = mode;
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

void displayTimeSpan(TimeSpan value, bool isFullTimeSpan, const char *title) {
  TimeSpanLabelArgs args {
    .value = value,
    .isFullTimeSpan = isFullTimeSpan,
    .label = title
  };
  TimeSpanLabel label(args);
  label.view();
}

void displayDateTime(DateTime value) {
  DateTimeLabelArgs args(value);
  DateTimeLabel label(args);
  label.view();
}

void changeSettings() {
  static const char menu_item_0[] PROGMEM = "Restart";
  static const char menu_item_1[] PROGMEM = "Default Irrigation Mode";
  static const char menu_item_2[] PROGMEM = "Cyclic Mode Settings";
  static const char menu_item_3[] PROGMEM = "Custom Mode Settings";
  static const char menu_item_4[] PROGMEM = "Pump Settings";
  static const char menu_item_5[] PROGMEM = "Adjust RTC";
  static const char menu_item_6[] PROGMEM = "Clear EEPROM";
  static const char* const menu[] PROGMEM = {
    menu_item_0,
    menu_item_1,
    menu_item_2,
    menu_item_3,
    menu_item_4,
    menu_item_5,
    menu_item_6
  };

  MenuArgs settingMenuArgs {
    .items = menu,
    .numItems = sizeof(menu) / sizeof(menu[0]),
    .defaultItemIndex = 0
  };
  Menu settingMenu(settingMenuArgs);
  
  while (true) {
    settingMenu.run();
    MenuResult settingMenuResult = settingMenu.getResult();
    if (settingMenu.isCanceled()) {
      return;
    }

    switch (settingMenuResult.selectedIndex){
      case 0:
        reset();
        break;
      case 1:
        showDefaultIrrigationMode();
        break;
      case 2:
        showCyclicModeSettings();
        break;
      case 3:
        showCustomModeSettings();
        break;
      case 4:
        showPumpSettings();
        break;
      case 5:
        showRTCAdjustment();
        break;
      case 6:
        resetEEPROM();
        break;
      default:
        break;
    }

    settingMenuArgs.defaultItemIndex = settingMenuResult.selectedIndex;
  }
}

void showDefaultIrrigationMode() {
  DefaultIrrigationModeArgs args;
  DefaultIrrigationMode setting(args);
  setting.run();
}

void showCyclicModeSettings() {
  CyclicModeSettingsArgs args;
  CyclicModeSettings settings(args);
  settings.run();
}

void showCustomModeSettings() {
  CustomModeEntriesArgs args;
  CustomModeEntries entries(args);
  entries.run();
}

void showPumpSettings() {
  PumpSettingsArgs args;
  PumpSettings settings(args);
  settings.run();
}

void showRTCAdjustment() {
  RtcAdjustmentArgs args;
  RtcAdjustment adjustment(args);
  adjustment.run();
}

void resetEEPROM() {
  EEPROMCleaningArgs args;
  EEPROMCleaning cleaning(args);
  cleaning.run();
}


/*
A: Always Off
B: Always On
C: Cyclic Mode: Pump will run in duty cycle of a full cycle, from start time to stop time.
D: Custom Mode: There are 24 slots for setting starting time and running duration of pump.
*/

#include <Wire.h>
#include <RTClib.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

RTC_DS3231 rtcDS3231;
RTC_Micros rtc;

hd44780_I2Cexp lcd;

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

const uint8_t TONE_PIN = 11;
const uint8_t PUMP_RELAY_PIN = 12;
const uint8_t VALVE_RELAY_PIN = 13;

const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte keypadRowPins[KEYPAD_ROWS] = { 10, 9, 8, 7 };
byte keypadColPins[KEYPAD_COLS] = { 6, 5, 4, 3 };

Keypad keypad = Keypad(makeKeymap(keys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

enum IrrigationMode {
  ALWAYS_OFF,
  ALWAYS_ON,
  CYCLIC,
  CUSTOM,
  REMOTE_CONTROL_MCU
};

enum DialogType {
  YESNO,
  OK
};

enum DisplayStatusType {
  DeviceTitle,
  CurrentDateTime,
  CurrentIrrigationMode,
  CurrentPumpStatus
};

bool isPumpRelayOn = false;
bool isValveRelayOn = false;

bool isPumpOn = false;
unsigned long pumpOperationChangedAt = 0;
bool safeToRunPumpMotor = false;
bool safeToCloseValve = false;

DateTime pumpStartedAt;
DateTime pumpStoppedAt;

enum IrrigationMode irrigationMode = ALWAYS_OFF;

const int EEPROM_CYCLIC_MODE_SETTINGS_ADDRESS  = 0;
const int EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS = EEPROM_CYCLIC_MODE_SETTINGS_ADDRESS + sizeof(uint32_t) * 4;
const int NUM_CUSTOM_MODE_SETTINGS = 24;
const int EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS + sizeof(uint32_t) * 2 * NUM_CUSTOM_MODE_SETTINGS;
const int EEPROM_PUMP_MOTOR_ENABLED_ADDRESS = EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS + sizeof(uint8_t);
const int EEPROM_PREVENT_WATER_HAMMER_WHEN_START_PUMP_SETTING_ADDRESS = EEPROM_PUMP_MOTOR_ENABLED_ADDRESS + sizeof(bool);
const int EEPROM_PREVENT_WATER_HAMMER_WHEN_STOP_PUMP_SETTING_ADDRESS = EEPROM_PREVENT_WATER_HAMMER_WHEN_START_PUMP_SETTING_ADDRESS + sizeof(uint16_t);

DateTime lastUpdateInternalRTC;
DateTime lastInteract;

// "99-23:59:59"
const char timeSpanFormat[] PROGMEM = "%02d-%02d:%02d:%02d";
const char timeSpanScanFormat[] PROGMEM = "%d-%d:%d:%d";
// "2022-31-01"
const char dateFormat[] PROGMEM = "%04d-%02d-%02d";
const char dateScanFormat[] PROGMEM = "%d-%d-%d";
// "23:59:59"
const char timeFormat[] PROGMEM = "%02d:%02d:%02d";
const char timeScanFormat[] PROGMEM = "%d:%d:%d";

const char dateLabel[] PROGMEM = "Date: ";
const char timeLabel[] PROGMEM = "Time: ";

const char prefixFormat[] PROGMEM = "%d: ";
const char twoNumberPrefixFormat[] PROGMEM = "%02d: ";

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

void setupRTC() {
  while (!rtcDS3231.begin()) {
    delay(100);
  }

  if (rtcDS3231.lostPower()) {
    rtcDS3231.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtcDS3231.clearAlarm(1);
  rtcDS3231.clearAlarm(2);
  rtcDS3231.disableAlarm(1);
  rtcDS3231.disableAlarm(2);
  rtcDS3231.writeSqwPinMode(DS3231_OFF);
  rtcDS3231.disable32K();
  
  rtc.begin(rtcDS3231.now());

  resetLastInternalRTCUpdate();
  resetLastInteractTimer();
}

void setupLCD() {
  int statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  while (statusCode != 0) {
    delay(100);
    statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  }
}

void setupKeypad() {
  keypad.addEventListener(keypadEventListender);
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

void resetLastInternalRTCUpdate() {
  lastUpdateInternalRTC = rtc.now();
}

bool doesInternalRTCNeedUpdateFromRTCModule() {
  if ((rtc.now() - lastUpdateInternalRTC).totalseconds() > 3600) {
    return true;
  }

  return false;
}

void keepInternalRTCUpToDate() {
  if (doesInternalRTCNeedUpdateFromRTCModule()) {
    rtc.adjust(rtcDS3231.now());
    resetLastInternalRTCUpdate();
  }
}

void resetLastInteractTimer() {
  lastInteract = rtc.now();
}

bool tooLongToLoop() {
  // Place this function into each infinite loop or keypad loop.
  // Each round of loop will reset WDT, idea for not placing wdt_reset too much in code.
  wdt_reset();

  if ((rtc.now() - lastInteract).totalseconds() > 3 * 60) {
    return true;
  }

  return false;
}

void turnOnPumpRelay() {
  if (!isPumpRelayOn) {
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    isPumpRelayOn = true;
  }
}

void turnOffPumpRelay() {
  if (isPumpRelayOn) {
    digitalWrite(PUMP_RELAY_PIN, LOW);
    isPumpRelayOn = false;
  }
}

void turnOnValveRelay() {
  if (!isValveRelayOn) {
    digitalWrite(VALVE_RELAY_PIN, HIGH);
    isValveRelayOn = true;
  }
}

void turnOffValveRelay() {
  if (isValveRelayOn) {
    digitalWrite(VALVE_RELAY_PIN, LOW);
    isValveRelayOn = false;
  }
}

void startPump() {
  if (!isPumpOn) {
    isPumpOn = true;
    pumpStartedAt = rtc.now();
    safeToRunPumpMotor = false;
    pumpOperationChangedAt = millis();
    highBeep();
  }

  turnOnValveRelay();

  if (!safeToRunPumpMotor) {
    uint16_t waterHammerDelay;
    EEPROM.get(EEPROM_PREVENT_WATER_HAMMER_WHEN_START_PUMP_SETTING_ADDRESS, waterHammerDelay);
    if (millis() - pumpOperationChangedAt >= waterHammerDelay || !safeToCloseValve) {
      safeToRunPumpMotor = true;
    }
  }

  bool enablePumpMotor;
  EEPROM.get(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, enablePumpMotor);
  if (!enablePumpMotor) {
    turnOffPumpRelay();
  } else {
    if (safeToRunPumpMotor) {
      turnOnPumpRelay();
    }
  }
}

void stopPump() {
  if (isPumpOn) {
    isPumpOn = false;
    pumpStoppedAt = rtc.now();
    safeToCloseValve = false;
    pumpOperationChangedAt = millis();
    highBeep();
  }

  turnOffPumpRelay();

  if (safeToCloseValve) {
    turnOffValveRelay();
  } else {
    uint16_t waterHammerDelay;
    EEPROM.get(EEPROM_PREVENT_WATER_HAMMER_WHEN_STOP_PUMP_SETTING_ADDRESS, waterHammerDelay);
    if (millis() - pumpOperationChangedAt >= waterHammerDelay || !safeToRunPumpMotor) {
      safeToCloseValve = true;
    }
  }
}

void beep() {
  tone(TONE_PIN, 5000, 100);
}

void lowBeep() {
  tone(TONE_PIN, 100, 100);
}

void highBeep() {
  tone(TONE_PIN, 10000, 300);
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
      displayTimeSpan(true, &(now - pumpStartedAt), PSTR("Pump: ON"));
    } else {
      displayTimeSpan(true, &(now - pumpStoppedAt), PSTR("Pump: OFF"));
    }
  } else {
    displayDateTime(&now);
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

  int selectedSetting = 0;
  int displaySetting = 0;
  bool loop = true;
  while (loop) {
    selectedSetting = showMenu(sizeof(menu) / sizeof(menu[0]), menu, displaySetting);
    if (selectedSetting >= 0) displaySetting = selectedSetting;
    switch (selectedSetting){
      case 0:
        reset();
        break;
      case 1:
        changeDefaultIrrigationMode();
        break;
      case 2:
        changeCyclicModeSettings();
        break;
      case 3:
        changeCustomModeSettings();
        break;
      case 4:
        changePumpSettings();
        break;
      case 5:
        adjustRTC();
        break;
      case 6:
        clearEEPROM();
        break;
      default:
        loop = false;
        break;
    }
  }
}

void changeDefaultIrrigationMode() {
  static const char menu_item_0[] PROGMEM = "[A] Turn Off";
  static const char menu_item_1[] PROGMEM = "[B] Always On";
  static const char menu_item_2[] PROGMEM = "[C] Cyclic";
  static const char menu_item_3[] PROGMEM = "[D] Custom";
  static const char* const menu[] PROGMEM = {
    menu_item_0,
    menu_item_1,
    menu_item_2,
    menu_item_3
  };

  const int numModes = sizeof(menu) / sizeof(menu[0]);

  uint8_t defaultMode = 0;
  EEPROM.get(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, defaultMode);
  if (defaultMode > numModes - 1) {
    defaultMode = 0;
    EEPROM.put(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, defaultMode);
  }

  int selectedMode = showMenu(numModes, menu, defaultMode);
  if (selectedMode < 0) return;
  if (selectedMode != defaultMode) {
    EEPROM.put(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, selectedMode);
  }
}

void changeCyclicModeSettings() {
    static const char menu_item_0[] PROGMEM = "Full Duration";
    static const char menu_item_1[] PROGMEM = "Duty Duration";
    static const char menu_item_2[] PROGMEM = "Start Time";
    static const char menu_item_3[] PROGMEM = "Stop Time";
    static const char* const menu[] PROGMEM = { 
      menu_item_0,
      menu_item_1,
      menu_item_2,
      menu_item_3
    };
    // ================================================================
    const int menuSize = sizeof(menu) / sizeof(menu[0]);
    
    bool loop = true;
    int selectedIndex = 0;
    while (loop) {
      selectedIndex = showMenu(menuSize, menu, selectedIndex);
      if (selectedIndex < 0) {
        break;
      }

      const bool FULL_TIMESPAN_LOOKUP[] = { true, true, false, false };
      const uint32_t MAX_LOOKUP[] = { 86400, 86400, 86400 - 1, 86400 - 1 };

      bool isFullTimeSpan = FULL_TIMESPAN_LOOKUP[selectedIndex];
      uint32_t max        = MAX_LOOKUP[selectedIndex];
      int eepromAddress = EEPROM_CYCLIC_MODE_SETTINGS_ADDRESS + sizeof(uint32_t) * selectedIndex;

      bool innerLoop = true;
      bool bypassGetKey = true;
      uint32_t eepromValue;
      TimeSpan timeSpan;
      while (innerLoop) {
        if (tooLongToLoop()) {
          break;
        }

        char key = keypad.getKey();
        if (!key && !bypassGetKey) continue;
        if (bypassGetKey) {
          key = '\0';
          bypassGetKey = false;
        }

        switch (key) {
          case '5': {
            bool isCanceled;
            while (true) {
              if (tooLongToLoop()) {
                isCanceled = true;
                break;
              }

              timeSpan = inputTimeSpan(isFullTimeSpan, &timeSpan, &isCanceled);
              if (isCanceled) {
                  break;
              }
              if (timeSpan.totalseconds() > max) {
                lowBeep();
                continue;
              }

              break;
            }

            if (!isCanceled) {
              EEPROM.put(eepromAddress, (uint32_t)timeSpan.totalseconds());
            }
          } break;
          case '*':
            innerLoop = false;
            break;
          default:
            break;
        }

        EEPROM.get(eepromAddress, eepromValue);
        timeSpan = TimeSpan(eepromValue);
        displayTimeSpan(isFullTimeSpan, &timeSpan, (const char *)pgm_read_ptr(&menu[selectedIndex]));
      }
    }
}

struct CustomModeSetting {
  uint32_t startTime;
  uint32_t duration;
};

void displayCustomSetting(int index, struct CustomModeSetting* setting) {
  restoreLcd();
  
  //  ________________
  // |24: 00:00:00    |
  // |    00:00:00    |
  //  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾

  const int bufferLength = 12;
  char buffer[bufferLength] = { '\0' };
  // bufferLength;
  snprintf_P(buffer, bufferLength, (const char *)(&twoNumberPrefixFormat[0]),
    index + 1);
  lcd.print(buffer);

  int padding = strnlen(buffer, bufferLength);

  TimeSpan ts1(setting->startTime);
  snprintf_P(buffer, bufferLength, (const char *)(&timeSpanFormat[0]),
    ts1.days(),
    ts1.hours(),
    ts1.minutes(),
    ts1.seconds());
  lcd.print(buffer);

  lcd.setCursor(padding, 1);
  TimeSpan ts2(setting->duration);
  snprintf_P(buffer, bufferLength, (const char *)(&timeSpanFormat[0]), 
    ts2.days(),
    ts2.hours(),
    ts2.minutes(),
    ts2.seconds());
  lcd.print(buffer);
}

void changeCustomModeSettings() {    
    restoreLcd();

    CustomModeSetting settings[NUM_CUSTOM_MODE_SETTINGS];
    
    int address = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS;
    for (int i = 0; i < NUM_CUSTOM_MODE_SETTINGS; ++i) {
      EEPROM.get(address, settings[i].startTime);
      address += sizeof(uint32_t);
      EEPROM.get(address, settings[i].duration);
      address += sizeof(uint32_t);
    }

    qsort(settings, NUM_CUSTOM_MODE_SETTINGS, sizeof(CustomModeSetting), compareCustomModeSetting);

    int index = 0;
    bool loop = true;
    bool bypassGetKey = true;
    while (loop) {
      if (tooLongToLoop()) {
        break;
      }

      char key = keypad.getKey();
      if (!bypassGetKey && !key) continue;
      if (bypassGetKey) {
        key = '\0';
        bypassGetKey = false;
      }

      switch (key) {
      case '2':
        --index;
        if (index < 0) {
          index = NUM_CUSTOM_MODE_SETTINGS - 1;
        }
        break;
      case '8':
        ++index;
        if (index > NUM_CUSTOM_MODE_SETTINGS - 1) {
          index = 0;
        }
        break;
      case '5': {
        struct CustomModeSetting newSetting = changeCustomModeSetting(index, &settings[index]);
        if (settings[index].startTime != newSetting.startTime || 
            settings[index].duration  != newSetting.duration) {
          
          address = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS;
          for (int i = 0; i < NUM_CUSTOM_MODE_SETTINGS; ++i) {
            uint32_t t1, t2;
            EEPROM.get(address                   , t1);
            EEPROM.get(address + sizeof(uint32_t), t2);

            if (t1 == settings[index].startTime && t2 == settings[index].duration) {
              EEPROM.put(address                   , newSetting.startTime);
              EEPROM.put(address + sizeof(uint32_t), newSetting.duration);
              break;
            }

            address += sizeof(uint32_t) * 2;
          }

          settings[index].startTime = newSetting.startTime;
          settings[index].duration  = newSetting.duration;

          qsort(settings, NUM_CUSTOM_MODE_SETTINGS, sizeof(CustomModeSetting), compareCustomModeSetting);

          // Adjusting display index.
          for (int i = 0; i < NUM_CUSTOM_MODE_SETTINGS; ++i) {
            if (settings[i].startTime == newSetting.startTime &&
                settings[i].duration == newSetting.duration) {
              index = i;
              break;
            }
          }
        }
      } break;
      case '*':
        loop = false;
        break;
      default:
        break;
      }

      displayCustomSetting(index, &settings[index]);
    }
}

struct CustomModeSetting changeCustomModeSetting(int index, struct CustomModeSetting* setting) {
  struct CustomModeSetting newSetting = *setting;

  int8_t lineIndex = 0;

  bool bypassGetKey = true;
  bool loop = true;
  bool cancel = false;
  while (loop) {
    if (tooLongToLoop()) {
      cancel = true;
      break;
    }

    char key = keypad.getKey();
    if (!key && !bypassGetKey) continue;
    if (bypassGetKey) {
      key = '\0';
      bypassGetKey = false;
    }

    switch(key) {
      case '2':
        --lineIndex;
        if (lineIndex < 0) {
          lineIndex = 1;
        }
        break;
      case '8':
        ++lineIndex;
        if (lineIndex > 1) {
          lineIndex = 0;
        }
        break;
      case '5': {
          bool isCanceled;
          TimeSpan newTimeSpan = lineIndex == 0 ? newSetting.startTime : newSetting.duration;
          bool isFullTimeSpan  = lineIndex == 0 ? false : true;
          uint32_t max         = lineIndex == 0 ? 86400 - 1 : 86400;
          while (true) {
            if (tooLongToLoop()) {
              isCanceled = true;
              break;
            }

            newTimeSpan = inputTimeSpan(isFullTimeSpan, &newTimeSpan, &isCanceled);
            if (isCanceled) {
              break;
            }

            if (newTimeSpan.totalseconds() > max) {
              lowBeep();
              continue;
            }

            break;
          }

          if (!isCanceled) {
            if (lineIndex == 0) {
              newSetting.startTime = newTimeSpan.totalseconds();
            } else {
              newSetting.duration = newTimeSpan.totalseconds();
            }
          }
      } break;
      case 'A':
        loop = false;
        break;
      case 'C':
        loop = false;
        cancel = true;
        break;
      default:
        break;
    }

    displayCustomSetting(index, &newSetting);
    lcd.setCursor(15, lineIndex);
    lcd.blink();
  }

  if (cancel) {
    newSetting = *setting;
  }

  return newSetting;
}

int compareCustomModeSetting(const void* a, const void* b) {
  const CustomModeSetting *_a = (const CustomModeSetting*)a;
  const CustomModeSetting *_b = (const CustomModeSetting*)b;

  if (_a->startTime == 0 && _a->duration == 0) {
    return 1;
  }

  if (_b->startTime == 0 && _b->duration == 0) {
    return -1;
  }

  if (_a->startTime > _b->startTime) {
    return 1;
  } else if (_a->startTime < _b->startTime) {
    return -1;
  } else {
    return 0;
  }
}

const char pump_settings_menu_item_0[] PROGMEM = "Pump Motor Enabled";
const char pump_settings_menu_Item_1[] PROGMEM = "Water Hammer Delay";
const char* const pump_settings_menu[] PROGMEM = {
  pump_settings_menu_item_0,
  pump_settings_menu_Item_1
};
void changePumpSettings() {
  int selectedSetting = 0;
  int displayedSetting = 0;
  bool loop = true;
  while (loop) {
    const int menuSize = sizeof(pump_settings_menu) / sizeof(pump_settings_menu[0]);
    selectedSetting = showMenu(menuSize, pump_settings_menu, displayedSetting);
    switch(selectedSetting) {
      case 0:
        changeEnablePumpMotorSetting();
        break;
      case 1:
        changeWaterHammerDelaySettings();
        break;
      default:
        loop = false;
        break;
    }

    displayedSetting = selectedSetting;
  }
}

void changeEnablePumpMotorSetting() {
  static const char menu_item_0[] PROGMEM = "No";
  static const char menu_item_1[] PROGMEM = "Yes";
  static const char* const menu[] PROGMEM = {
    menu_item_0,
    menu_item_1
  };
  bool defaultSetting;
  EEPROM.get(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, defaultSetting);

  int selected = showMenu(sizeof(menu) / sizeof(menu[0]), menu, (int)defaultSetting);
  if (selected < 0) return;

  EEPROM.put(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, (bool) selected);
}

void changeWaterHammerDelaySettings() {
  static const char water_hammer_delay_when_start_pump[] PROGMEM = "When start pump";
  static const char water_hammer_delay_when_stop_pump[] PROGMEM = "When stop pump";
  static const char* const water_hammer_delay_menu[] PROGMEM = {
    water_hammer_delay_when_start_pump,
    water_hammer_delay_when_stop_pump
  };

  const int menuSize = sizeof(water_hammer_delay_menu) / sizeof(water_hammer_delay_menu[0]);

  int selectedItem = 0;
  bool loop = true;
  while (loop) {
    selectedItem = showMenu(menuSize, water_hammer_delay_menu, selectedItem);
    uint16_t defaultValue;
    int selectedEepromAddress;
    if (selectedItem == 0) {
      selectedEepromAddress = EEPROM_PREVENT_WATER_HAMMER_WHEN_START_PUMP_SETTING_ADDRESS;
    } else if (selectedItem == 1) {
      selectedEepromAddress = EEPROM_PREVENT_WATER_HAMMER_WHEN_STOP_PUMP_SETTING_ADDRESS;
    } else {
      break;
    }

    changeWaterHammerDelaySetting(selectedEepromAddress);
  }
}

void changeWaterHammerDelaySetting(int eepromAddress) {
  const uint16_t MAX_VALUE = 60000;
  
  const int bufferSize = LCD_COLS + 1;
  char buffer[bufferSize] = { '\0' };

  bool loop = true;
  bool skipKeyPad = true;
  while (loop) {
    if (tooLongToLoop()) {
      break;
    }

    char key = keypad.getKey();
    if (!key && !skipKeyPad) {
      continue;
    }
    if (skipKeyPad) {
      skipKeyPad = false;
      key = '\0';
    }

    uint16_t currentValue;
    EEPROM.get(eepromAddress, currentValue);
    switch (key) {
      case '5': {
        uint32_t value = currentValue;
        while (true) {
          if (tooLongToLoop()) {
            break;
          }

          bool isCanceled;
          value = inputUnsignedInteger(value, &isCanceled);
          if (isCanceled) break;

          if (value > MAX_VALUE) {
            lowBeep();
            continue;
          }

          currentValue = (uint16_t)value;
          EEPROM.put(eepromAddress, currentValue);
          break;
        }
      } break;
      case '*':
        loop = false;
        break;
      default:
        break;
    }

    snprintf_P(buffer, bufferSize, PSTR("[0; %u] ms"), MAX_VALUE);
    restoreLcd();
    lcd.print(buffer);
    lcd.setCursor(0, 1);
    lcd.print(currentValue);
  }
}

void adjustRTC() {
  restoreLcd();
  
  rtc.adjust(rtcDS3231.now());

  DateTime displayTime = rtc.now();
  bool redraw = true;
  bool loop = true;
  while(loop) {
    if (tooLongToLoop()) {
      break;
    }

    DateTime evalNow = rtc.now();
    if (evalNow != displayTime || redraw) {
      if (redraw) {
        lcd.clear();
        redraw = false;
      }
      displayTime = evalNow;
      displayDateTime(&displayTime);
    }

    char key = keypad.getKey();    
    if (!key) continue;

    switch(key) {
      case '5': {
        DateTime defaultValue = rtc.now();
        DateTime adjustedNow = inputDateTime(&defaultValue);
        if (adjustedNow != defaultValue) {
          rtc.adjust(adjustedNow);
          rtcDS3231.adjust(rtc.now());
        }
        redraw = true;
      } break;
      case '*':
        loop = false;
        break;
      default:
        break;
    }
  }

  resetLastInternalRTCUpdate();
  resetLastInteractTimer();
}

void clearEEPROM() {
  static const char warningMessage[] PROGMEM = "Do you want to continue?";
  static const char finishMessage[]  PROGMEM = "Success!";
  if (displayDialog(warningMessage, YESNO)){
    for (int i = 0; i < EEPROM.length(); ++i) {
      EEPROM.write(i, 0);
    }

    displayDialog(finishMessage, OK);
  }
}

bool isDateTimeDelimiter(char character) {
  return character == '-' || character == ':';
}

TimeSpan inputTimeSpan(bool fullTimeSpan, TimeSpan *defaultValue, bool *isCanceled) {
  restoreLcd();
  lcd.cursor();

  const int CUSOR_PADDING = 4;

  const int bufferLength = 12;
  char input[bufferLength] = { '\0' };
  
  if (fullTimeSpan) {
    lcd.print(F("Set DD-HH:MM:SS"));
  } else {
    lcd.print(F("Set HH:MM:SS"));
  }

  lcd.setCursor(0, 1);
  if (fullTimeSpan) {
    snprintf_P(input, bufferLength, (const char *)(&timeSpanFormat[0]),
    defaultValue->days(),
    defaultValue->hours(),
    defaultValue->minutes(),
    defaultValue->seconds());
  } else {
    // Reuse time format, because it is the same as short time span format.
    snprintf_P(input, bufferLength, (const char *)(&timeFormat[0]),
    defaultValue->hours(),
    defaultValue->minutes(),
    defaultValue->seconds());
  }

  int dd, hh, mm, ss;

  int index = 0;

  bool loop = true;
  bool cancel = false;
  bool bypassGetKey = true;
  while (loop) {
    if (tooLongToLoop()) {
      cancel = true;
      break;
    }

    char key = keypad.getKey();
    if (!bypassGetKey && !key) continue;
    if (bypassGetKey) {
      bypassGetKey = false;
    }

    switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        input[index] = key;
        if (index < strlen(input) - 1) {
          ++index;

          if (isDateTimeDelimiter(input[index])) {
            ++index;
          }
        }
        break;
      case '*':
        --index;
        if (isDateTimeDelimiter(input[index])) {
          --index;
        }
        if (index < 0) {
          index = strlen(input) - 1;
        }
        break;
      case '#':
        ++index;
        if (isDateTimeDelimiter(input[index])) {
          ++index;
        }
        if (index >= strlen(input)) {
          index = 0;
        }
        break;
      case 'A':
        if (fullTimeSpan) {
          sscanf_P(input, (const char *)(&timeSpanScanFormat[0]),
          &dd, &hh, &mm, &ss);
        } else {
          sscanf_P(input, (const char *)(&timeScanFormat[0]),
          &hh, &mm, &ss);
          dd = 0;
        }
        if (dd < 0 || dd > 99 || hh < 0 || hh > 23 || mm < 0 || mm > 59 || ss < 0 || ss > 59) {
          lowBeep();
        } else {
          loop = false;
        }
        break;
      case 'C':
        loop = false;
        cancel = true;
        break;
      default:
        break;
    }

    lcd.setCursor(CUSOR_PADDING, 1);
    lcd.print(input);
    lcd.setCursor(CUSOR_PADDING + index, 1);
  }

  restoreLcd();
  if (isCanceled != NULL) {
    *isCanceled = cancel;
  }
  if (cancel) {
    return *defaultValue;
  }
  return TimeSpan(dd, hh, mm, ss);
}

TimeSpan inputFullTimeSpan(TimeSpan *defaultValue, bool *isCanceled) {
  return inputTimeSpan(true, defaultValue, isCanceled);
}

TimeSpan inputShortTimeSpan(TimeSpan *defaultValue, bool *isCanceled) {
  return inputTimeSpan(false, defaultValue, isCanceled);
}

DateTime inputDateTime(DateTime* defaultValue) {
  restoreLcd();
  lcd.cursor();

  lcd.setCursor(0, 0);
  lcd.print((const __FlashStringHelper *)(&dateLabel[0]));

  lcd.setCursor(0, 1);  
  lcd.print((const __FlashStringHelper *)(&timeLabel[0]));

  const int8_t CUSOR_PADDING = 6;

  const int inputBufferLength = 12;
  char input0[inputBufferLength] = { '\0' };
  char input1[inputBufferLength] = { '\0' };

  char temp[5] = { '\0' };

  snprintf_P(input0, inputBufferLength, (const char *)(&dateFormat[0]), 
    defaultValue->year(),
    defaultValue->month(),
    defaultValue->day());
  
  snprintf_P(input1, inputBufferLength, (const char *)(&timeFormat[0]),
    defaultValue->hour(),
    defaultValue->minute(),
    defaultValue->second());

  int year, month, day, hour, minute, second;

  int index0 = 0;
  int index1 = 0;
  int lineIndex = 0;

  bool loop = true;
  bool cancel = false;
  bool bypassGetKey = true;
  while (loop) {
    if (tooLongToLoop()) {
      cancel = true;
      break;
    }

    char key = keypad.getKey();
    if (!bypassGetKey && !key) continue;
    if (bypassGetKey) {
      key = '\0';
      bypassGetKey = false;
    }

    char * const input = lineIndex == 0 ? input0 : input1;
    int  * const index = lineIndex == 0 ? &index0 : &index1;
    bool edgeOfLine = false;
    switch(key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        input[*index] = key;
        if (*index < strlen(input) - 1) {
          ++*index;

          if (isDateTimeDelimiter(input[*index])) {
            ++*index;
          }
        } else {
          edgeOfLine = true;
        }

        if (lineIndex == 0 && edgeOfLine) {
          lineIndex = 1;
          index1 = 0;
        }
        break;
      case '*':
        --*index;
        if (isDateTimeDelimiter(input[*index])) {
          --*index;
        }
        if (*index < 0) {
          edgeOfLine = true;
        }

        if (edgeOfLine) {
          if (lineIndex == 0) {
            lineIndex = 1;
            index1 = strlen(input1) - 1;
          } else {
            lineIndex = 0;
            index0 = strlen(input0) - 1;
          }
        }
        break;
      case '#':
        ++*index;
        if (isDateTimeDelimiter(input[*index])) {
          ++*index;
        }
        if (*index >= strlen(input)) {
          edgeOfLine = true;
        }

        if (edgeOfLine) {
          if (lineIndex == 0) {
            lineIndex = 1;
            index1 = 0;
          } else {
            lineIndex = 0;
            index0 = 0;
          }
        }
        break;
      case 'A':
        sscanf_P(input0, (const char *)(&dateScanFormat[0]),
          &year, &month, &day);
        
        sscanf_P(input1, (const char *)(&timeScanFormat[0]),
          &hour, &minute, &second);

        if (!DateTime(year, month, day, hour, minute, second).isValid()) {
          lowBeep();
        } else {
          loop = false;
        }
        break;
      case 'C':
        loop = false;
        cancel = true;
        break;
      default:
        break;
    }

    lcd.setCursor(CUSOR_PADDING, 0);
    lcd.print(input0);
    lcd.setCursor(CUSOR_PADDING, 1);
    lcd.print(input1);

    if (lineIndex == 0) {
      lcd.setCursor(CUSOR_PADDING + index0, 0);
    } else {
      lcd.setCursor(CUSOR_PADDING + index1, 1);
    }
  }
  
  if (cancel) return *defaultValue;
  return DateTime(year, month, day, hour, minute, second);
}

uint32_t inputUnsignedInteger(uint32_t defaultValue, bool* isCanceled) {
  restoreLcd();
  
  lcd.print(F("Enter value:"));
  lcd.cursor();

  const int NUMBER_LENGTH = 10;
  const int INPUT_BUFFER_LENGTH = NUMBER_LENGTH + 1;
  char input[INPUT_BUFFER_LENGTH] = { '\0' };
  memset(input, '\0', INPUT_BUFFER_LENGTH);
  if (defaultValue != 0) {
    snprintf_P(input, INPUT_BUFFER_LENGTH, PSTR("%lu"), defaultValue);
  }
  
  int index = 0;
  bool loop = true;
  bool cancel = false;
  bool skipKeyPad = true;
  while (loop) {
    if (tooLongToLoop()) {
      cancel = true;
      break;
    }

    char key = keypad.getKey();
    if (!key && !skipKeyPad) {
      continue;
    }
    if (skipKeyPad) {
      skipKeyPad = false;
      key = '\0';
    }

    switch (key)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      // 01234567890
      // 2138084789N
      // 2111111111N
      memmove(&input[index + 1], &input[index], strlen(&input[index]));
      input[NUMBER_LENGTH] = '\0';
      input[index] = key;
      
      if (index < NUMBER_LENGTH - 1) {
        ++index;
      }
      break;
    case '*':
      --index;
      if (index < 0) {
        index = strlen(input);
        if (index >= NUMBER_LENGTH) {
          index = NUMBER_LENGTH - 1;
        }
      }
      break;
    case '#':
      ++index;
      if (index > strlen(input) || index >= NUMBER_LENGTH) {
        index = 0;
      }
      break;
    case 'B':
      if (index > 0) {
        memmove(&input[index - 1], &input[index], strlen(&input[index]) + 1);
        --index;
      }
      break;
    case 'D':
      if (index < strlen(input)) {
        memmove(&input[index], &input[index + 1], strlen(&input[index + 1]) + 1);
      }
      break;
    case 'A':
      loop = false;
      break;
    case 'C':
      loop = false;
      cancel = true;
      break;
    default:
      break;
    }

    memset(&input[strlen(input)], '\0', INPUT_BUFFER_LENGTH - strlen(input));
    clearLcdRow(1);
    lcd.setCursor(0, 1);
    lcd.print(input);
    lcd.setCursor(index, 1);
  }

  if (isCanceled != NULL) {
    *isCanceled = cancel;
  }
  if (cancel) return defaultValue;

  uint32_t value = strtoul(input, NULL, 10);
  return value;
}

void displayDateTime(DateTime* dateTime) {
  restoreLcd();

  /*
      ________________
    |Date: 9999-12-31|
    |Time: 23:59:59  |
      ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  */
  const int bufferLength = 12;
  char output[bufferLength] = { '\0' };

  lcd.print((const __FlashStringHelper *)(&dateLabel[0]));
  snprintf_P(output, bufferLength, (const char *)(&dateFormat[0]),
    dateTime->year(),
    dateTime->month(),
    dateTime->day());
  lcd.print(output);

  lcd.setCursor(0, 1);
  lcd.print((const __FlashStringHelper *)(&timeLabel[0]));
  snprintf_P(output, bufferLength, (const char *)(&timeFormat[0]),
    dateTime->hour(),
    dateTime->minute(),
    dateTime->second());
  lcd.print(output);
}

void displayTimeSpan(bool isFullTimeSpan, TimeSpan* timeSpan, const char *label) {
  restoreLcd();

  lcd.print((const __FlashStringHelper *)(&label[0]));

  const int bufferLength = 12;
  char buffer[bufferLength] = { '\0' };

  if (isFullTimeSpan) {
    snprintf_P(buffer, bufferLength, (const char *)(&timeSpanFormat[0]),
      timeSpan->days(),
      timeSpan->hours(),
      timeSpan->minutes(),
      timeSpan->seconds());
  } else {
    snprintf_P(buffer, bufferLength, (const char *)(&timeFormat[0]),
      timeSpan->hours(),
      timeSpan->minutes(),
      timeSpan->seconds());
  }

  lcd.setCursor(0, 1);
  lcd.print(buffer);
}

int showMenu(int numItems, const char* const *menu, int defaultItemIndex) {
  restoreLcd();

  int itemIndex = defaultItemIndex;

  const int bufferLength = LCD_COLS + 1;
  char buffer[bufferLength] = { '\0' };
  
  const char *menuItem = NULL;
  bool menuItemChanged = true;

  const int maxMenuItemWordWrapHistory = 10;
  const char* menuItemWordWrapHistory[maxMenuItemWordWrapHistory] = { NULL };
  int menuItemWordWrapHistoryIndex = 0;

  bool loop = true;
  bool cancel = false;
  bool bypassGetKey = true;
  while (loop) {
    if (tooLongToLoop()) {
      cancel = true;
      break;
    }

    char key = keypad.getKey();
    if (!key && !bypassGetKey) continue;
    if (bypassGetKey) {
      key = '\0';
      bypassGetKey = false;
    }

    switch (key) {
      case '2':
        --itemIndex;
        if (itemIndex < 0) {
          itemIndex = numItems - 1;
        }
        menuItemChanged = true;
        break;
      case '8':
        ++itemIndex;
        if (itemIndex > numItems - 1) {
          itemIndex = 0;
        }
        menuItemChanged = true;
        break;
      case '4':
        --menuItemWordWrapHistoryIndex;
        if (menuItemWordWrapHistoryIndex < 0) {
          menuItemWordWrapHistoryIndex = 0;
        }
        break;
      case '6':
        ++menuItemWordWrapHistoryIndex;
        if (menuItemWordWrapHistoryIndex > maxMenuItemWordWrapHistory - 1) {
          menuItemWordWrapHistoryIndex = maxMenuItemWordWrapHistory - 1;
        }
        break;
      case '5':
        loop = false;
        break;
      case '*':
        cancel = true;
        loop = false;
        break;
      default:
        break;
    }

    lcd.clear();
    snprintf_P(buffer, bufferLength, (const char *)(&prefixFormat[0]), itemIndex + 1);
    int prefixLength = strlen(buffer);
    lcd.print(buffer);

    if (menuItemChanged) {
      menuItemChanged = false;
      menuItem = (const char *)pgm_read_ptr(&menu[itemIndex]);
      for (int i = 0; i < maxMenuItemWordWrapHistory; ++i) {
        menuItemWordWrapHistory[i] = NULL;
      }
      menuItemWordWrapHistory[0] = menuItem;
    }

    const char *startToWrap = menuItemWordWrapHistory[menuItemWordWrapHistoryIndex];
    if (startToWrap == NULL) {
      --menuItemWordWrapHistoryIndex;
      startToWrap = menuItemWordWrapHistory[menuItemWordWrapHistoryIndex];
    }

    startToWrap = wordWrap(startToWrap, buffer, bufferLength - prefixLength);
    lcd.setCursor(prefixLength, 0);
    lcd.print(buffer);
    startToWrap = wordWrap(startToWrap, buffer, bufferLength);
    lcd.setCursor(0, 1);
    lcd.print(buffer);

    const int nextMenuItemWordWrapHistoryIndex = menuItemWordWrapHistoryIndex + 1;
    if (nextMenuItemWordWrapHistoryIndex < maxMenuItemWordWrapHistory) {
      menuItemWordWrapHistory[nextMenuItemWordWrapHistoryIndex] = startToWrap;
    }
  }

  if (cancel) {
    return -1;
  }

  return itemIndex;
}

int8_t displayDialog(const char* message, enum DialogType dialogType) {
  restoreLcd();

  static const char selection_yesno_str[]   PROGMEM = "(C)No (A)Yes";
  static const char selection_ok_str[]      PROGMEM = "(A)OK";
  static const char selection_unknown_str[] PROGMEM = "(A)!@#%*";

  const int bufferSize = LCD_COLS + 1;
  char buffer[bufferSize] = { '\0' };
  
  const int maxMessageWordWrapHistory = 10;
  const char* messageWordWrapHistory[maxMessageWordWrapHistory] = { NULL };
  int messageWordWrapHistoryIndex = 0;
  
  messageWordWrapHistory[0] = message;

  bool loop = true;
  bool cancel = false;
  bool bypassGetKey = true;
  while (loop) {
    if (tooLongToLoop()) {
      cancel = true;
      break;
    }

    char key = keypad.getKey();
    if (!key && !bypassGetKey) continue;
    if (bypassGetKey) {
      key = '\0';
      bypassGetKey = false;
    }

    switch (key) {
      case '4':
        --messageWordWrapHistoryIndex;
        if (messageWordWrapHistoryIndex < 0) {
          messageWordWrapHistoryIndex = 0;
        }
        break;
      case '6':
        ++messageWordWrapHistoryIndex;
        if (messageWordWrapHistoryIndex > maxMessageWordWrapHistory - 1) {
          messageWordWrapHistoryIndex = maxMessageWordWrapHistory - 1;
        }
        break;
      case 'A':
        loop = false;
        break;
      case 'C':
        if (dialogType == YESNO) {
          loop = false;
          cancel = true;
        }
        break;
      default:
        break;
    }
    
    const char *startToWrap = messageWordWrapHistory[messageWordWrapHistoryIndex];
    if (startToWrap == NULL) {
      --messageWordWrapHistoryIndex;
      startToWrap = messageWordWrapHistory[messageWordWrapHistoryIndex];
    }
    startToWrap = wordWrap(startToWrap, buffer, bufferSize);
    lcd.clear();
    lcd.print(buffer);

    const int nextMessageWordWrapHistoryIndex = messageWordWrapHistoryIndex + 1;
    if (nextMessageWordWrapHistoryIndex < maxMessageWordWrapHistory) {
      messageWordWrapHistory[nextMessageWordWrapHistoryIndex] = startToWrap;
    }
    
    const char *actionsStr = NULL;
    switch (dialogType) {
      case YESNO:
        actionsStr = selection_yesno_str;
        break;
      case OK:
        actionsStr =  selection_ok_str;
        break;
      default:
        actionsStr = selection_unknown_str;
        break;
    }

    const int actionsStrLen = strlen_P(actionsStr);
    // AVR LIBC does not support "%*s" format in sprintf/snprintf.
    lcd.setCursor(0, 1);
    for (int i = 0; i < (LCD_COLS - actionsStrLen) / 2; ++i) {
      lcd.print(' ');
    }
    lcd.print((const __FlashStringHelper *)actionsStr);
    for (int i = 0; i < (LCD_COLS - actionsStrLen) / 2; ++i) {
      lcd.print(' ');
    }
  }

  if (cancel) return 0;
  return 1;
}

void restoreLcd() {
  lcd.clear();
  lcd.noLineWrap();
  lcd.noCursor();
  lcd.noBlink();
}

void clearLcdRow(int row) {
  lcd.setCursor(0, row);
  for (int i = 0; i < LCD_COLS; ++i) {
    lcd.print(' ');
  }
}

const char *wordWrap(const char *src, char *buffer, const int bufferSize) {
  memset(buffer, '\0', bufferSize);
  
  const int bufferMaxNumCharacters = bufferSize - 1;

  int currentLength = 0;
  while (true) {
    const char *startOfWord = NULL;
    const char *endOfWord = NULL;
    startOfWord = findStartOfWord(src);
    if (startOfWord == NULL) {
      return NULL;
    }
    endOfWord = findEndOfWord(startOfWord);
    if (endOfWord == NULL) {
      return NULL;
    }

    int wordLength = endOfWord - startOfWord + 1;

    bool paddingSpace = currentLength != 0;
    if (currentLength + (int)paddingSpace + wordLength <= bufferMaxNumCharacters) {
      if (paddingSpace) {
        buffer[currentLength] = ' ';
        ++currentLength;
      }

      strncat_P(buffer, startOfWord, wordLength);
      currentLength += wordLength;
      src = startOfWord + wordLength;
    } else {
      if (wordLength > bufferMaxNumCharacters) {
        if (currentLength == 0) {
          const int numChars = bufferMaxNumCharacters;
          strncat_P(buffer, startOfWord, numChars);
          src = startOfWord + numChars;
        }
      } else {
        src = startOfWord;
      }
      break;
    }
  }

  return src;
}

const char *findStartOfWord(const char *src) {
  if (src == NULL) return NULL;

  while (true) {
    unsigned char character = pgm_read_byte(src);
    
    if (character == '\0') {
      return NULL;
    }

    if (character != ' ') {
      return src;
    }

    ++src;
  }
}

const char *findEndOfWord(const char *src) {
  if (src == NULL) return NULL;

  unsigned char character = pgm_read_byte(src);
  if (character == '\0') {
    return NULL;
  }

  if (character == ' ') {
    return src;
  }

  while (true) {
    ++src;

    character = pgm_read_byte(src);
    if (character == ' ' || character == '\0') {
      return src - 1;
    }
  }
}

void keypadEventListender(KeypadEvent key) {
  switch (keypad.getState())
  {
  case PRESSED:
    resetLastInteractTimer();
    beep();
    break;
  case RELEASED:
    break;
  case HOLD:
    lowBeep();
    break;
  default:
    break;
  }
}

/*
A: Always Off
B: Always On
C: Cyclic Mode: Pump will run in duty cycle of a full cycle, from start time to stop time.
D: Custom Mode: There are 24 slots for setting starting time and running duration of pump.
*/

#include <avr/wdt.h>

#include "src/devices/rtc/rtc.hpp"
#include "src/devices/lcd/lcd.hpp"
#include "src/devices/keypad/keypad.hpp"
#include "src/devices/pump/pump.hpp"
#include "src/devices/buzzer/buzzer.hpp"

#include "src/irrigation/irrigation.hpp"

#include "src/inputs/too-long-to-loop/too-long-to-loop.hpp"
#include "src/labels/system-status/system-status.hpp"

#include "src/settings/settings.hpp"

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  Serial.println(F("Working..."));

  wdt_enable(WDTO_8S);

  setupRTC();
  setupLCD();
  setupTooLongToLoop();
  
  setupRelays();
  setupPumpTimers();

  setupDefaultIrrigationMode();

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

/*
void setupInterrupts() {
  pinMode(D1307_SQW_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(D1307_SQW_INTERRUPT_PIN), D1307SwqIsr, CHANGE);
}
*/

void chooseFunction() {
  char key = keypad.getKey();

  if (!key) {
    return;
  }

  switch (key) {
    case 'A':
      changeIrrigationMode(ALWAYS_OFF);
      setDisplayStatus(CurrentIrrigationMode);
      break;
    case 'B':
      changeIrrigationMode(ALWAYS_ON);
      setDisplayStatus(CurrentIrrigationMode);
      break;
    case 'C':
      changeIrrigationMode(CYCLIC);
      setDisplayStatus(CurrentIrrigationMode);
      break;
    case 'D':
      changeIrrigationMode(CUSTOM);
      setDisplayStatus(CurrentIrrigationMode);
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

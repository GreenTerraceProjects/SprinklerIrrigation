#include "too-long-to-loop.hpp"
#include "../../devices/rtc/rtc.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../devices/buzzer/buzzer.hpp"
#include <avr/wdt.h>

DateTime lastInteract;

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

void setupTooLongToLoop() {
  resetLastInteractTimer();
  keypad.addEventListener(keypadEventListender);
}

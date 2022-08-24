#include "watch-dog.hpp"
#include <avr/wdt.h>
#include "../rtc/rtc.hpp"

DateTime lastInteract;

bool tooLongToLoop() {
  // Place this function into each infinite loop or keypad loop.
  // Each round of loop will reset WDT, idea for not placing wdt_reset too much in code.
  wdt_reset();

  if ((rtc.now() - lastInteract).totalseconds() > 3 * 60) {
    return true;
  }

  return false;
}

void resetLastInteractTimer() {
  lastInteract = rtc.now();
}

#include "rtc-adjustment.hpp"
#include "../../inputs/too-long-to-loop/too-long-to-loop.hpp"
#include "../../inputs/datetime-input/datetime-input.hpp"
#include "../../labels/datetime-label/datetime-label.hpp"
#include "../../devices/rtc/rtc.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"

void adjustRTC() {
  restoreLcd();

  rtc.adjust(rtcDS3231.now());

  DateTime displayTime = rtc.now();
  bool refreshDisplayImmediately = true;
  bool loop = true;
  while(loop) {
    if (tooLongToLoop()) {
      break;
    }

    DateTime evalNow = rtc.now();
    if (evalNow != displayTime || refreshDisplayImmediately) {
      if (refreshDisplayImmediately) {
        clearLcd();
        refreshDisplayImmediately = false;
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

          resetLastInternalRTCUpdate();
          resetLastInteractTimer();
        }
        refreshDisplayImmediately = true;
      } break;
      case '*':
        loop = false;
        break;
      default:
        break;
    }
  }
}

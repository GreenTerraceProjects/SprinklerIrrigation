#include "custom-mode-entry.hpp"
#include "../../../inputs/too-long-to-loop/too-long-to-loop.hpp"
#include "../../../inputs/timespan-input/timespan-input.hpp"
#include "../../../strings/formats/number-formats.hpp"
#include "../../../strings/formats/datetime-formats.hpp"
#include "../../../devices/keypad/keypad.hpp"
#include "../../../devices/buzzer/buzzer.hpp"
#include "../../../devices/lcd/lcd.hpp"

void changeCustomModeSetting(int sortedIndex, uint32_t &startAt, uint32_t &duration, bool &isCanceled) {
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
          TimeSpan newTimeSpan = lineIndex == 0 ? startAt : duration;
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
              startAt = newTimeSpan.totalseconds();
            } else {
              duration = newTimeSpan.totalseconds();
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

    displayCustomSetting(sortedIndex, startAt, duration);
    lcd.setCursor(15, lineIndex);
    lcd.blink();
  }

  isCanceled = cancel;
}

void displayCustomSetting(int sortedIndex, const uint32_t &startAt, const uint32_t &duration) {
  restoreLcd();
  
  //  ________________
  // |24: 00:00:00    |
  // |    00:00:00    |
  //  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾

  const int bufferLength = 12;
  char buffer[bufferLength] = { '\0' };
  // bufferLength;
  snprintf_P(buffer, bufferLength, (const char *)(&twoNumberPrefixFormat[0]),
    sortedIndex + 1);
  lcd.print(buffer);

  int padding = strnlen(buffer, bufferLength);

  TimeSpan ts1(startAt);
  snprintf_P(buffer, bufferLength, (const char *)(&timeSpanFormat[0]),
    ts1.days(),
    ts1.hours(),
    ts1.minutes(),
    ts1.seconds());
  lcd.print(buffer);

  lcd.setCursor(padding, 1);
  TimeSpan ts2(duration);
  snprintf_P(buffer, bufferLength, (const char *)(&timeSpanFormat[0]), 
    ts2.days(),
    ts2.hours(),
    ts2.minutes(),
    ts2.seconds());
  lcd.print(buffer);
}

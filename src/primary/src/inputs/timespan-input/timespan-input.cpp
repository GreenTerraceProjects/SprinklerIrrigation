#include "timespan-input.hpp"
#include <stdio.h>
#include <string.h>
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../devices/buzzer/buzzer.hpp"
#include "../../strings/datetime-utils/datetime-utils.hpp"
#include "../../strings/formats/datetime-formats.hpp"
#include "../keypad-utils/keypad-utils.hpp"

TimeSpan inputFullTimeSpan(TimeSpan *defaultValue, bool *isCanceled) {
  return inputTimeSpan(true, defaultValue, isCanceled);
}

TimeSpan inputShortTimeSpan(TimeSpan *defaultValue, bool *isCanceled) {
  return inputTimeSpan(false, defaultValue, isCanceled);
}

TimeSpan inputTimeSpan(bool fullTimeSpan, TimeSpan *defaultValue, bool *isCanceled) {
  restoreLcd();
  lcd.cursor();

  const int CUSOR_PADDING = 4;

  const int bufferLength = 12;
  char input[bufferLength] = { };
  
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
    char key = '\0';
    if (bypassGetKey) {
      bypassGetKey = false;
    } else {
      if (!waitForKeyPressed(key)) {
        cancel = true;
        break;
      }
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

  if (isCanceled != NULL) {
    *isCanceled = cancel;
  }
  if (cancel) {
    return *defaultValue;
  }

  return TimeSpan(dd, hh, mm, ss);
}

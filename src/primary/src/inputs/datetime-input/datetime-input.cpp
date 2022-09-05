#include "datetime-input.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../devices/buzzer/buzzer.hpp"
#include "../../strings/formats/datetime-formats.hpp"
#include "../../strings/labels/datetime-labels.hpp"
#include "../../strings/datetime-utils/datetime-utils.hpp"
#include "../keypad-utils/keypad-utils.hpp"

DateTime inputDateTime(DateTime* defaultValue) {
  restoreLcd();
  lcd.cursor();

  lcd.setCursor(0, 0);
  lcd.print((const __FlashStringHelper *)(&dateLabel[0]));

  lcd.setCursor(0, 1);  
  lcd.print((const __FlashStringHelper *)(&timeLabel[0]));

  const int8_t CUSOR_PADDING = 6;

  const int inputBufferLength = 12;
  char input0[inputBufferLength] = { };
  char input1[inputBufferLength] = { };

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
    char key = '\0';
    if (bypassGetKey) {
      bypassGetKey = false;
    } else {
      if (!waitForKeyPressed(key)) {
        cancel = true;
        break;
      }
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

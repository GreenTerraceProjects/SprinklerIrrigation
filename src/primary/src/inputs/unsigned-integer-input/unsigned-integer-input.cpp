#include "unsigned-integer-input.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../keypad-utils/keypad-utils.hpp"

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
    char key = '\0';
    if (skipKeyPad) {
      skipKeyPad = false;
    } else {
      if (!waitForKeyPressed(key)) {
        cancel = true;
        break;
      }
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

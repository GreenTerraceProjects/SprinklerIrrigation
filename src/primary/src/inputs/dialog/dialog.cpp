#include "dialog.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../strings/wordwrap/wordwrap.hpp"
#include "../keypad-utils/keypad-utils.hpp"

const char selection_yesno_str[]   PROGMEM = "(C)No (A)Yes";
const char selection_ok_str[]      PROGMEM = "(A)OK";
const char selection_unknown_str[] PROGMEM = "(A)!@#%*";

int8_t displayDialog(const char* message, enum DialogType dialogType) {
  restoreLcd();

  const int bufferSize = LCD_COLS + 1;
  char buffer[bufferSize] = { };
  
  WordWrap wordWrap(message, buffer, bufferSize);
  int messageWordWrapLineIndex = 0;

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
      case '4':
        --messageWordWrapLineIndex;
        if (messageWordWrapLineIndex < 0) {
          messageWordWrapLineIndex = 0;
        }
        break;
      case '6':
        ++messageWordWrapLineIndex;
        if (messageWordWrapLineIndex > wordWrap.getTotalLines() - 1) {
          messageWordWrapLineIndex = wordWrap.getTotalLines() - 1;
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
    
    wordWrap.getLine(messageWordWrapLineIndex);
    clearLcd();
    lcd.print(buffer);
    
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

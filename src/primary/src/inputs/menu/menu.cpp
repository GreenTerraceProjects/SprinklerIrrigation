#include "menu.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../strings/wordwrap/wordwrap.hpp"
#include "../../strings/formats/number-formats.hpp"
#include "../keypad-utils/keypad-utils.hpp"

int showMenu(int numItems, const char* const *menu, int defaultItemIndex) {
  restoreLcd();

  int itemIndex = defaultItemIndex;

  const int bufferLength = LCD_COLS + 1;
  char buffer[bufferLength] = { };
  const int prefixLength = 5;
  char prefix[prefixLength] = { };
  
  const char *menuItem = NULL;
  bool menuItemChanged = true;

  byte wordWordBuff[sizeof(WordWrap)] = { };
  WordWrap *wordWrap = NULL;
  int wordWrapLineIndex;

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
        wordWrapLineIndex -= LCD_ROWS;
        if (wordWrapLineIndex < 0) {
          wordWrapLineIndex = 0;
        }
        break;
      case '6':
        wordWrapLineIndex += LCD_ROWS;
        if (wordWrapLineIndex >= wordWrap->getTotalLines()) {
          wordWrapLineIndex = 0;
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

    if (menuItemChanged) {
      menuItemChanged = false;
      menuItem = (const char *)pgm_read_ptr(&menu[itemIndex]);

      snprintf_P(prefix, prefixLength, (const char *)(&prefixFormat[0]), itemIndex + 1);
      wordWrap = new(wordWordBuff)WordWrap(menuItem, buffer, bufferLength, prefix, LCD_ROWS);
      wordWrapLineIndex = 0;
    }

    wordWrap->getLine(wordWrapLineIndex);
    clearLcd();
    lcd.print(buffer);
    wordWrap->getLine(wordWrapLineIndex + 1);
    lcd.setCursor(0, 1);
    lcd.print(buffer);
  }

  if (cancel) {
    return -1;
  }

  return itemIndex;
}

#include <avr/pgmspace.h>
#include "menu.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../strings/formats/number-format-strings.hpp"

Menu::Menu(MenuArgs& args) : args(args), Input(&keypad) {

}

void Menu::initialize() {
  this->menuItemIndex = this->args.defaultItemIndex;
  this->menuItemChanged = true;
  this->menuItemPageStartingLine = 1;
  this->menuItemWordWrapMovingDirection = WordWrapMovingDirection::NotChange;
  restoreLcd();
  this->refreshDisplay();
}

bool Menu::handleKeyPressed(char inputKey) {
  switch (inputKey) {
     case '2':
        --this->menuItemIndex;
        if (this->menuItemIndex < 0) {
          this->menuItemIndex = this->args.numItems - 1;
        }
        this->menuItemChanged = true;
        break;
      case '8':
        ++this->menuItemIndex;
        if (this->menuItemIndex > this->args.numItems - 1) {
          this->menuItemIndex = 0;
        }
        this->menuItemChanged = true;
        break;
      case '4':
        this->menuItemChanged = WordWrapMovingDirection::Backward;
        break;
      case '6':
        this->menuItemChanged = WordWrapMovingDirection::Forward;
        break;
      case '5':
        return false;
        break;
      case '*':
        this->cancel();
        return false;
      default:
        break;
  }

  this->refreshDisplay();
  return true;
}

void Menu::refreshDisplay() {
  const int PREFIX_BUFFER_LENGTH = 6;
  const int OUTPUT_BUFFER_LENGTH = LCD_COLS + 1;
  char buffer[OUTPUT_BUFFER_LENGTH] = { '\0' };
  char prefix[PREFIX_BUFFER_LENGTH] = { '\0' };

  snprintf_P(prefix, PREFIX_BUFFER_LENGTH, (const char *)(&prefixFormat[0]), this->menuItemIndex + 1);

  const char *menuItem = (const char *)pgm_read_ptr(&(this->args.items[this->menuItemIndex]));
  if (this->menuItemChanged) {
    this->menuItemChanged = false;
    this->menuItemPageStartingLine = 0;
  }

  PrefixWordWrap prefixWordWrap(menuItem, buffer, OUTPUT_BUFFER_LENGTH, prefix, LCD_ROWS);
  if (this->menuItemWordWrapMovingDirection == WordWrapMovingDirection::Forward) {
    if (prefixWordWrap.goTo(this->menuItemPageStartingLine + LCD_ROWS)) {
      this->menuItemPageStartingLine += LCD_ROWS;
    }
  } else if (this->menuItemWordWrapMovingDirection == WordWrapMovingDirection::Backward) {
    if (prefixWordWrap.goTo(this->menuItemPageStartingLine - LCD_ROWS)) {
      this->menuItemPageStartingLine -= LCD_ROWS;
    }
  } else {
    prefixWordWrap.goTo(this->menuItemPageStartingLine);
  }

  if (this->menuItemWordWrapMovingDirection != WordWrapMovingDirection::NotChange) {
    this->menuItemWordWrapMovingDirection = WordWrapMovingDirection::NotChange;
  }

  lcd.clear();
  lcd.print(buffer);
  if (prefixWordWrap.goTo(this->menuItemPageStartingLine + 1)) {
    lcd.setCursor(0, 1);
    lcd.print(buffer);
  }
}

MenuResult Menu::getResult() {
  MenuResult result;
  if (this->isCanceled()) {
    result.selectedIndex = -1;
  } else {
    result.selectedIndex = this->menuItemIndex;
  }  
  return result;
}

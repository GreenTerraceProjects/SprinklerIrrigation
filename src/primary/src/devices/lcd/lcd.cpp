#include "lcd.hpp"
#include <avr/pgmspace.h>

hd44780_I2Cexp lcd;

void restoreLcd() {
  clearLcd();
  lcd.noLineWrap();
  lcd.noCursor();
  lcd.noBlink();
}

void fillSpace(char *src, int size) {
  for (int index = 0; index < size - 1; ++index) {
    src[index] = ' ';
  }
  src[size - 1] = '\0';
}

void clearLcd() {
  const int blankSize = LCD_COLS + 1;
  char blank[blankSize];
  fillSpace(blank, blankSize);

  for (uint8_t rowIndex = 0; rowIndex < LCD_ROWS; ++rowIndex) {
    lcd.setCursor(0, rowIndex);
    lcd.print(blank);
  }

  lcd.setCursor(0, 0);
}

void clearLcdRow(int row) {
  const int blankSize = LCD_COLS + 1;
  char blank[blankSize];
  fillSpace(blank, blankSize);

  lcd.setCursor(0, row);
  lcd.print(blank);
  lcd.setCursor(0, row);
}

void setupLCD() {
  int statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  while (statusCode != 0) {
    delay(100);
    statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  }
}

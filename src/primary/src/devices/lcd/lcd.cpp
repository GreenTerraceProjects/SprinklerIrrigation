#include "lcd.hpp"
#include <avr/pgmspace.h>

hd44780_I2Cexp lcd;

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

void restoreLcd() {
  lcd.clear();
  lcd.noLineWrap();
  lcd.noCursor();
  lcd.noBlink();
}

void clearCld() {
  char blank[LCD_COLS + 1] = { ' ' };
  blank[LCD_COLS] = '\0';

  for (int row = 0; row < LCD_ROWS; ++row) {
    lcd.setCursor(0, row);
    lcd.print(blank);
  }
}

void clearLcdRow(int row) {
  char blank[LCD_COLS + 1] = { ' ' };
  blank[LCD_COLS] = '\0';

  lcd.setCursor(0, row);
  lcd.print(blank);
}

void setupLCD() {
  int statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  while (statusCode != 0) {
    delay(100);
    statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  }
}

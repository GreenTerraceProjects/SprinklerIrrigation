#include "lcd.hpp"

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

hd44780_I2Cexp lcd;

void setupLCD() {
  int statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  while (statusCode != 0) {
    delay(100);
    statusCode = lcd.begin(LCD_COLS, LCD_ROWS);
  }
}

void restoreLcd() {
  lcd.clear();
  lcd.noLineWrap();
  lcd.noCursor();
  lcd.noBlink();
}

void clearLcdRow(int row) {
  lcd.setCursor(0, row);
  for (int i = 0; i < LCD_COLS; ++i) {
    lcd.print(' ');
  }
}

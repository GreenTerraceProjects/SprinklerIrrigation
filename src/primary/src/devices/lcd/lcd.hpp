#ifndef _LCD_HPP_
#define _LCD_HPP_

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

extern hd44780_I2Cexp lcd;

extern const uint8_t LCD_COLS;
extern const uint8_t LCD_ROWS;

void restoreLcd();
void clearLcd();
void clearLcdRow(int row);
void setupLCD();

#endif

#ifndef _LCD_HPP_
#define _LCD_HPP_

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <stdint.h>

extern hd44780_I2Cexp lcd;

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

void restoreLcd();
void clearLcd();
void clearLcdRow(int row);
void setupLCD();

#endif

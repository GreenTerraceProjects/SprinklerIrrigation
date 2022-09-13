#ifndef _LCD_SHADOW_HPP_
#define _LCD_SHADOW_HPP_

#include "lcd.hpp"

class LcdShadow {
public:
    LcdShadow(hd44780_I2Cexp &lcd);

protected:
    static const size_t BUFFER_SIZE = LCD_COLS * LCD_ROWS;
    hd44780_I2Cexp &lcd;
    char buffer[BUFFER_SIZE];
    uint8_t col;
    uint8_t row;

public:
    void setCursor(uint8_t col, uint8_t row);
    void print(const char *src);
    void print(const __FlashStringHelper *src);

public:
    void clear();

public:
    void commit();
};

extern LcdShadow lcdShadow;

#endif

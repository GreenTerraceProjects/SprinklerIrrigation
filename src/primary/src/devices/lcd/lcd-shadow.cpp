#include "lcd-shadow.hpp"
#include <string.h>
#include <stddef.h>
#include <avr/pgmspace.h>

LcdShadow lcdShadow(lcd);

LcdShadow::LcdShadow(hd44780_I2Cexp &lcd) : lcd(lcd) {
    this->clear();
}

void LcdShadow::setCursor(uint8_t col, uint8_t row) {
    this->col = col;
    this->row = row;
}

void LcdShadow::print(const char *src) {
    size_t index = this->row * LCD_COLS + this->col;
    for (; index < this->BUFFER_SIZE; ++index) {
        unsigned char character = *src++;
        if (character == '\0') break;

        this->buffer[index] = character;
    }

    uint8_t row = index / LCD_COLS;
    uint8_t col = index % LCD_COLS;
    this->setCursor(col, row);
}

void LcdShadow::print(const __FlashStringHelper *src) {
    const char *pgmSrc = reinterpret_cast<const char *>(src);

    size_t index = this->row * LCD_COLS + this->col;
    for (; index < this->BUFFER_SIZE; ++index) {
        unsigned char character = pgm_read_byte(pgmSrc++);
        if (character == '\0') break;

        this->buffer[index] = character;
    }

    uint8_t row = index / LCD_COLS;
    uint8_t col = index % LCD_COLS;
    this->setCursor(col, row);
}

void LcdShadow::clear() {
    memset(this->buffer, ' ', sizeof(char) * BUFFER_SIZE);
    this->setCursor(0, 0);
}

void LcdShadow::commit() {
    this->lcd.setCursor(0, 0);
    for (size_t index = 0; index < BUFFER_SIZE; ++index) {        
        this->lcd.print(this->buffer[index]);
        
        uint8_t row = index / LCD_COLS;
        uint8_t col = index % LCD_COLS;
        if (++col >= LCD_COLS) {
            col = 0;
            
            if (++row >= LCD_ROWS) {
                row = 0;
            }

            this->lcd.setCursor(col, row);
        }
    }
}

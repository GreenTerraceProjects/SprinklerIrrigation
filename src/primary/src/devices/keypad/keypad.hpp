#ifndef _KEYPAD_HPP_
#define _KEYPAD_HPP_

#include <Keypad.h>

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

extern char keys[KEYPAD_ROWS][KEYPAD_COLS];

extern byte keypadRowPins[KEYPAD_ROWS];
extern byte keypadColPins[KEYPAD_COLS];

extern Keypad keypad;

#endif

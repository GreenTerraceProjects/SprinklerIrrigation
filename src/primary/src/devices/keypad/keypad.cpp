#include "keypad.hpp"

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte keypadRowPins[KEYPAD_ROWS] = { 10, 9, 8, 7 };
byte keypadColPins[KEYPAD_COLS] = { 6, 5, 4, 3 };

Keypad keypad = Keypad(makeKeymap(keys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

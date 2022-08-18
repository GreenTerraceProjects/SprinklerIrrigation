#include "keypad.hpp"
#include "../buzzer/buzzer.hpp"

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte keypadRowPins[KEYPAD_ROWS];
byte keypadColPins[KEYPAD_COLS];

Keypad keypad = Keypad(makeKeymap(keys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

void keypadEventListender(KeypadEvent key) {
  switch (keypad.getState())
  {
  case PRESSED:
    beep();
    break;
  case RELEASED:
    break;
  case HOLD:
    lowBeep();
    break;
  default:
    break;
  }
}

void setupKeypad() {
  keypad.addEventListener(keypadEventListender);
}

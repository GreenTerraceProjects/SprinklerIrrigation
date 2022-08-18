#include "buzzer.hpp"
#include "../pins/pins.hpp"

#include <Arduino.h>

void beep() {
  tone(TONE_PIN, 5000, 100);
}

void lowBeep() {
  tone(TONE_PIN, 100, 100);
}

void highBeep() {
  tone(TONE_PIN, 10000, 300);
}

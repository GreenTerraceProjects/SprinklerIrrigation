#include "relays.hpp"
#include "../pins/pins.hpp"

#include <Arduino.h>

bool isPumpRelayOn = false;
bool isValveRelayOn = false;

void turnOnPumpRelay() {
  if (!isPumpRelayOn) {
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    isPumpRelayOn = true;
  }
}

void turnOffPumpRelay() {
  if (isPumpRelayOn) {
    digitalWrite(PUMP_RELAY_PIN, LOW);
    isPumpRelayOn = false;
  }
}

void turnOnValveRelay() {
  if (!isValveRelayOn) {
    digitalWrite(VALVE_RELAY_PIN, HIGH);
    isValveRelayOn = true;
  }
}

void turnOffValveRelay() {
  if (isValveRelayOn) {
    digitalWrite(VALVE_RELAY_PIN, LOW);
    isValveRelayOn = false;
  }
}

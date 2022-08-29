#include <Arduino.h>
#include <EEPROM.h>
#include "pump.hpp"
#include "../buzzer/buzzer.hpp"
#include "../rtc/rtc.hpp"
#include "../pins/pins.hpp"
#include "../eeprom/eeprom-addresses.hpp"

bool isPumpRelayOn = false;
bool isValveRelayOn = false;

bool isPumpOn = false;
unsigned long pumpOperationChangedAt = 0;
bool safeToRunPumpMotor = false;
bool safeToCloseValve = false;

DateTime pumpStartedAt;
DateTime pumpStoppedAt;

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

void startPump() {
  if (!isPumpOn) {
    isPumpOn = true;
    pumpStartedAt = rtc.now();
    safeToRunPumpMotor = false;
    pumpOperationChangedAt = millis();
    highBeep();
  }

  turnOnValveRelay();

  if (!safeToRunPumpMotor) {
    uint16_t waterHammerDelay;
    EEPROM.get(EEPROM_PREVENT_WATER_HAMMER_WHEN_START_PUMP_SETTING_ADDRESS, waterHammerDelay);
    if (millis() - pumpOperationChangedAt >= waterHammerDelay || !safeToCloseValve) {
      safeToRunPumpMotor = true;
    }
  }

  bool enablePumpMotor;
  EEPROM.get(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, enablePumpMotor);
  if (!enablePumpMotor) {
    turnOffPumpRelay();
  } else {
    if (safeToRunPumpMotor) {
      turnOnPumpRelay();
    }
  }
}

void stopPump() {
  if (isPumpOn) {
    isPumpOn = false;
    pumpStoppedAt = rtc.now();
    safeToCloseValve = false;
    pumpOperationChangedAt = millis();
    highBeep();
  }

  turnOffPumpRelay();

  if (safeToCloseValve) {
    turnOffValveRelay();
  } else {
    uint16_t waterHammerDelay;
    EEPROM.get(EEPROM_PREVENT_WATER_HAMMER_WHEN_STOP_PUMP_SETTING_ADDRESS, waterHammerDelay);
    if (millis() - pumpOperationChangedAt >= waterHammerDelay || !safeToRunPumpMotor) {
      safeToCloseValve = true;
    }
  }
}

void setupRelays() {
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(VALVE_RELAY_PIN, OUTPUT);
  turnOffPumpRelay();
  turnOffValveRelay();
}

void setupPumpTimers() {
  pumpStartedAt = pumpStoppedAt = rtc.now();
}

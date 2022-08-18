#include <EEPROM.h>
#include "pump.hpp"
#include "../relays/relays.hpp"
#include "../rtc/rtc.hpp"
#include "../buzzer/buzzer.hpp"
#include "../eeprom/addresses.hpp"

bool isPumpOn = false;
unsigned long pumpOperationChangedAt = 0;
bool safeToRunPumpMotor = false;
bool safeToCloseValve = false;

DateTime pumpStartedAt;
DateTime pumpStoppedAt;

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
    EEPROM.get(EEPROM_PREVENT_WATER_HAMMER_SETTING_ADDRESS, waterHammerDelay);
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
    EEPROM.get(EEPROM_PREVENT_WATER_HAMMER_SETTING_ADDRESS + sizeof(uint32_t), waterHammerDelay);
    if (millis() - pumpOperationChangedAt >= waterHammerDelay || !safeToRunPumpMotor) {
      safeToCloseValve = true;
    }
  }
}

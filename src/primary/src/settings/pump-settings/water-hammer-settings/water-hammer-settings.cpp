#include "water-hammer-settings.hpp"
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "../../../inputs/menu/menu.hpp"
#include "../../../inputs/unsigned-integer-input/unsigned-integer-input.hpp"
#include "../../../inputs/keypad-utils/keypad-utils.hpp"
#include "../../../inputs/too-long-to-loop/too-long-to-loop.hpp"
#include "../../../devices/eeprom/eeprom-addresses.hpp"
#include "../../../devices/lcd/lcd.hpp"
#include "../../../devices/keypad/keypad.hpp"
#include "../../../devices/buzzer/buzzer.hpp"

void changeWaterHammerDelaySetting(int eepromAddress);

const char water_hammer_delay_when_start_pump[] PROGMEM = "When start pump";
const char water_hammer_delay_when_stop_pump[] PROGMEM = "When stop pump";
const char* const water_hammer_delay_menu[] PROGMEM = {
  water_hammer_delay_when_start_pump,
  water_hammer_delay_when_stop_pump
};
void changeWaterHammerDelaySettings() {
  const int menuSize = sizeof(water_hammer_delay_menu) / sizeof(water_hammer_delay_menu[0]);

  int selectedItem = 0;
  bool loop = true;
  while (loop) {
    selectedItem = showMenu(menuSize, water_hammer_delay_menu, selectedItem);
    uint16_t defaultValue;
    int selectedEepromAddress;
    if (selectedItem == 0) {
      selectedEepromAddress = EEPROM_PREVENT_WATER_HAMMER_WHEN_START_PUMP_SETTING_ADDRESS;
    } else if (selectedItem == 1) {
      selectedEepromAddress = EEPROM_PREVENT_WATER_HAMMER_WHEN_STOP_PUMP_SETTING_ADDRESS;
    } else {
      break;
    }

    changeWaterHammerDelaySetting(selectedEepromAddress);
  }
}

void changeWaterHammerDelaySetting(int eepromAddress) {
  const uint16_t MAX_VALUE = 60000;
  
  const int bufferSize = LCD_COLS + 1;
  char buffer[bufferSize] = { '\0' };

  bool loop = true;
  bool skipKeyPad = true;
  while (loop) {
    char key = '\0';
    if (skipKeyPad) {
      skipKeyPad = false;
    } else {
      if (!waitForKeyPressed(key)) {
        break;
      }
    }

    uint16_t currentValue;
    EEPROM.get(eepromAddress, currentValue);
    switch (key) {
      case '5': {
        uint32_t value = currentValue;
        while (true) {
          if (tooLongToLoop()) {
            break;
          }

          bool isCanceled;
          value = inputUnsignedInteger(value, &isCanceled);
          if (isCanceled) break;

          if (value > MAX_VALUE) {
            lowBeep();
            continue;
          }

          currentValue = (uint16_t)value;
          EEPROM.put(eepromAddress, currentValue);
          break;
        }
      } break;
      case '*':
        loop = false;
        break;
      default:
        break;
    }

    snprintf_P(buffer, bufferSize, PSTR("[0; %u] ms"), MAX_VALUE);
    restoreLcd();
    lcd.print(buffer);
    lcd.setCursor(0, 1);
    lcd.print(currentValue);
  }
}

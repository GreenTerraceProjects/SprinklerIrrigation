#include "default-irrigation-mode.hpp"
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "../../devices/eeprom/eeprom-addresses.hpp"
#include "../../inputs/menu/menu.hpp"

const char menu_item_0[] PROGMEM = "[A] Turn Off";
const char menu_item_1[] PROGMEM = "[B] Always On";
const char menu_item_2[] PROGMEM = "[C] Cyclic";
const char menu_item_3[] PROGMEM = "[D] Custom";
const char* const menu[] PROGMEM = {
  menu_item_0,
  menu_item_1,
  menu_item_2,
  menu_item_3
};

void changeDefaultIrrigationMode() {
  const int numModes = sizeof(menu) / sizeof(menu[0]);

  uint8_t defaultMode = 0;
  EEPROM.get(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, defaultMode);
  if (defaultMode > numModes - 1) {
    defaultMode = 0;
    EEPROM.put(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, defaultMode);
  }

  int selectedMode = showMenu(numModes, menu, defaultMode);
  if (selectedMode < 0) return;
  if (selectedMode != defaultMode) {
    EEPROM.put(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, selectedMode);
  }
}

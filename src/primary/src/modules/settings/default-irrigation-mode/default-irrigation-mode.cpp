#include "default-irrigation-mode.hpp"
#include "../../inputs/menu/menu.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../devices/eeprom/addresses.hpp"
#include <avr/pgmspace.h>
#include <EEPROM.h>

DefaultIrrigationMode::DefaultIrrigationMode()
  : Input(&keypad) {

}

static const char menu_item_0[] PROGMEM = "[A] Turn Off";
static const char menu_item_1[] PROGMEM = "[B] Always On";
static const char menu_item_2[] PROGMEM = "[C] Cyclic";
static const char menu_item_3[] PROGMEM = "[D] Custom";
static const char* const menu[] PROGMEM = {
  menu_item_0,
  menu_item_1,
  menu_item_2,
  menu_item_3
};

void DefaultIrrigationMode::initialize() {
  const int numModes = sizeof(menu) / sizeof(menu[0]);

  uint8_t defaultMode = this->readDefaultIrrigationMode();
  if (defaultMode > numModes - 1) {
    defaultMode = 0;
  }

  MenuArgs menuArgs {
    .items = menu,
    .numItems = numModes,
    .defaultItemIndex = defaultMode
  };
  Menu menu(menuArgs);
  menu.run();
  if (menu.isCanceled()) return;
  
  MenuResult menuResult = menu.getResult();
  this->writeDefaultIrriationMode(menuResult.selectedIndex);
}

bool DefaultIrrigationMode::handleKeyPressed(char inputKey) {
  return false;
}

uint8_t DefaultIrrigationMode::readDefaultIrrigationMode() {
  uint32_t value;
  EEPROM.get(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, value);
  return value;
}

void DefaultIrrigationMode::writeDefaultIrriationMode(uint8_t value) {
  EEPROM.put(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, value);
}

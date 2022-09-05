#include "pump-motor-availability.hpp"
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "../../../devices/eeprom/eeprom-addresses.hpp"
#include "../../../inputs/menu/menu.hpp"

const char menu_item_0[] PROGMEM = "Disabled";
const char menu_item_1[] PROGMEM = "Enabled";
const char* const menu[] PROGMEM = {
  menu_item_0,
  menu_item_1
};

void changePumpMotorAvailabilitySetting() {
  bool defaultSetting;
  EEPROM.get(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, defaultSetting);

  int selected = showMenu(sizeof(menu) / sizeof(menu[0]), menu, (int)defaultSetting);
  if (selected < 0) return;

  EEPROM.put(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, (bool) selected);
}

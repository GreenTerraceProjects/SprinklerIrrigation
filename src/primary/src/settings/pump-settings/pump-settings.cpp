#include "pump-settings.hpp"
#include <avr/pgmspace.h>
#include "pump-motor-availability/pump-motor-availability.hpp"
#include "water-hammer-settings/water-hammer-settings.hpp"
#include "../../inputs/menu/menu.hpp"

const char pump_settings_menu_item_0[] PROGMEM = "Pump Motor Availability";
const char pump_settings_menu_Item_1[] PROGMEM = "Water Hammer Prevention";
const char* const pump_settings_menu[] PROGMEM = {
  pump_settings_menu_item_0,
  pump_settings_menu_Item_1
};
void changePumpSettings() {
  int selectedSetting = 0;
  int displayedSetting = 0;
  bool loop = true;
  while (loop) {
    const int menuSize = sizeof(pump_settings_menu) / sizeof(pump_settings_menu[0]);
    selectedSetting = showMenu(menuSize, pump_settings_menu, displayedSetting);
    switch(selectedSetting) {
      case 0:
        changePumpMotorAvailabilitySetting();
        break;
      case 1:
        changeWaterHammerPreventionSettings();
        break;
      default:
        loop = false;
        break;
    }

    displayedSetting = selectedSetting;
  }
}

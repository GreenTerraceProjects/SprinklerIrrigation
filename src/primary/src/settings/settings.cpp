#include "../inputs/menu/menu.hpp"
#include "../devices/lcd/lcd.hpp"
#include "default-irrigation-mode/default-irrigation-mode.hpp"
#include "cyclic-mode-settings/cyclic-mode-settings.hpp"
#include "custom-mode-entries/custom-mode-entries.hpp"
#include "pump-settings/pump-settings.hpp"
#include "rtc-adjustment/rtc-adjustment.hpp"
#include "eeprom-cleaning/eeprom-cleaning.hpp"
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <Arduino.h>

void reset() {
  clearLcd();
  lcd.print(F("Restarting ..."));
  lcd.blink();
  
  wdt_disable();
  wdt_enable(WDTO_2S);
  wdt_reset();
  for (;;) {}
}

const char menu_item_0[] PROGMEM = "Restart";
const char menu_item_1[] PROGMEM = "Default Irrigation Mode";
const char menu_item_2[] PROGMEM = "Cyclic Mode Settings";
const char menu_item_3[] PROGMEM = "Custom Mode Settings";
const char menu_item_4[] PROGMEM = "Pump Settings";
const char menu_item_5[] PROGMEM = "Adjust RTC";
const char menu_item_6[] PROGMEM = "Clear EEPROM";
const char* const menu[] PROGMEM = {
  menu_item_0,
  menu_item_1,
  menu_item_2,
  menu_item_3,
  menu_item_4,
  menu_item_5,
  menu_item_6
};

void changeSettings() {
  int selectedMenuItemIndex = 0;
  int defaultMenuItemIndex = 0;
  bool loop = true;
  while (loop) {
    selectedMenuItemIndex = showMenu(sizeof(menu) / sizeof(menu[0]), menu, defaultMenuItemIndex);
    if (selectedMenuItemIndex >= 0) {
      defaultMenuItemIndex = selectedMenuItemIndex;
    }

    switch (selectedMenuItemIndex){
      case 0:
        reset();
        break;
      case 1:
        changeDefaultIrrigationMode();
        break;
      case 2:
        changeCyclicModeSettings();
        break;
      case 3:
        changeCustomModeSettings();
        break;
      case 4:
        changePumpSettings();
        break;
      case 5:
        adjustRTC();
        break;
      case 6:
        clearEEPROM();
        break;
      default:
        loop = false;
        break;
    }
  }
}

#include "cyclic-mode-settings.hpp"
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "../../inputs/menu/menu.hpp"
#include "../../inputs/timespan-input/timespan-input.hpp"
#include "../../inputs/keypad-utils/keypad-utils.hpp"
#include "../../inputs/too-long-to-loop/too-long-to-loop.hpp"
#include "../../labels/timespan-label/timespan-label.hpp"
#include "../../devices/buzzer/buzzer.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../devices/eeprom/eeprom-addresses.hpp"

const char menu_item_0[] PROGMEM = "Full Duration";
const char menu_item_1[] PROGMEM = "Duty Duration";
const char menu_item_2[] PROGMEM = "Start Time";
const char menu_item_3[] PROGMEM = "Stop Time";
const char* const menu[] PROGMEM = { 
  menu_item_0,
  menu_item_1,
  menu_item_2,
  menu_item_3
};

void changeCyclicModeSettings() {
    // ================================================================
    const int menuSize = sizeof(menu) / sizeof(menu[0]);
    
    bool loop = true;
    int selectedIndex = 0;
    while (loop) {
      selectedIndex = showMenu(menuSize, menu, selectedIndex);
      if (selectedIndex < 0) {
        break;
      }

      const bool FULL_TIMESPAN_LOOKUP[] = { true, true, false, false };
      const uint32_t MAX_LOOKUP[] = { 86400, 86400, 86400 - 1, 86400 - 1 };

      bool isFullTimeSpan = FULL_TIMESPAN_LOOKUP[selectedIndex];
      uint32_t max        = MAX_LOOKUP[selectedIndex];
      int eepromAddress = EEPROM_CYCLIC_MODE_SETTINGS_ADDRESS + sizeof(uint32_t) * selectedIndex;

      bool innerLoop = true;
      bool bypassGetKey = true;
      uint32_t eepromValue;
      TimeSpan timeSpan;
      while (innerLoop) {
        char key = '\0';
        if (bypassGetKey) {
          bypassGetKey = false;
        } else {
          if (!waitForKeyPressed(key)) {
            break;
          }
        }

        switch (key) {
          case '5': {
            bool isCanceled;
            while (true) {
              if (tooLongToLoop()) {
                isCanceled = true;
                break;
              }

              timeSpan = inputTimeSpan(isFullTimeSpan, &timeSpan, &isCanceled);
              if (isCanceled) {
                  break;
              }
              if (timeSpan.totalseconds() > max) {
                lowBeep();
                continue;
              }

              break;
            }

            if (!isCanceled) {
              EEPROM.put(eepromAddress, (uint32_t)timeSpan.totalseconds());
            }
          } break;
          case '*':
            innerLoop = false;
            break;
          default:
            break;
        }

        EEPROM.get(eepromAddress, eepromValue);
        timeSpan = TimeSpan(eepromValue);
        displayTimeSpan(isFullTimeSpan, &timeSpan, (const char *)pgm_read_ptr(&menu[selectedIndex]));
      }
    }
}

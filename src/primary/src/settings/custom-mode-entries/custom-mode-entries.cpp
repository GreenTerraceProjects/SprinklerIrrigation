#include "custom-mode-entries.hpp"
#include "custom-mode-entry/custom-mode-entry.hpp"
#include <EEPROM.h>
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/eeprom/eeprom-addresses.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../inputs/keypad-utils/keypad-utils.hpp"

void readEntry(int8_t index, uint32_t &startAt, uint32_t &duration) {
  int address = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS + sizeof(uint32_t) * 2 * index;
  EEPROM.get(address, startAt);
  address += sizeof(uint32_t);
  EEPROM.get(address, duration);
}

void writeEntry(int8_t index, const uint32_t &startAt, const uint32_t &duration) {
  int address = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS + sizeof(uint32_t) * 2 * index;
  EEPROM.put(address, startAt);
  address += sizeof(uint32_t);
  EEPROM.put(address, duration);
}

int compareTwoEntries(const void* a, const void* b) {
  const int8_t *_a = (const int8_t *)a;
  const int8_t *_b = (const int8_t *)b;

  uint32_t entryA_startAt;
  uint32_t entryA_duration;
  uint32_t entryB_startAt;
  uint32_t entryB_duration;
  readEntry(*_a, entryA_startAt, entryA_duration);
  readEntry(*_b, entryB_startAt, entryB_duration);

  if (entryA_startAt == 0 && entryA_duration == 0 ||
      entryB_startAt == 0 && entryB_duration == 0) {
    return 1;
  }

  if (entryA_startAt > entryB_startAt) {
    return 1;
  } else if (entryA_startAt < entryB_startAt) {
    return -1;
  } else {
    if (entryA_duration > entryB_duration) {
      return 1;
    } else if (entryA_duration < entryB_duration) {
      return -1;
    } else {
      return 0;
    }
  }
}

void changeCustomModeSettings() {    
    restoreLcd();

    int8_t entries[NUM_CUSTOM_MODE_SETTINGS];
    for (int8_t index = 0; index < NUM_CUSTOM_MODE_SETTINGS; ++index) {
      entries[index] = index;
    }

    qsort(entries, NUM_CUSTOM_MODE_SETTINGS, sizeof(int8_t), compareTwoEntries);

    int sortedIndex = 0;
    bool loop = true;
    bool bypassGetKey = true;
    while (loop) {
      char key = '\0';
      if (bypassGetKey) {
        bypassGetKey = false;
      } else {
        if (!waitForKeyPressed(key)) {
          break;
        }
      }

      switch (key) {
      case '2':
        --sortedIndex;
        if (sortedIndex < 0) {
          sortedIndex = NUM_CUSTOM_MODE_SETTINGS - 1;
        }
        break;
      case '8':
        ++sortedIndex;
        if (sortedIndex > NUM_CUSTOM_MODE_SETTINGS - 1) {
          sortedIndex = 0;
        }
        break;
      case '5': {
        uint32_t startAt;
        uint32_t duration;
        readEntry(entries[sortedIndex], startAt, duration);
        bool isCanceled;
        changeCustomModeSetting(sortedIndex, startAt, duration, isCanceled);
        if (!isCanceled) {
          writeEntry(entries[sortedIndex], startAt, duration);
        }

        const int8_t realIndex = entries[sortedIndex];
        qsort(entries, NUM_CUSTOM_MODE_SETTINGS, sizeof(int8_t), compareTwoEntries);
        
        for (int index = 0; index < NUM_CUSTOM_MODE_SETTINGS; ++index) {
          if (entries[index] == realIndex) {
            sortedIndex = index;
            break;
          }
        }

      } break;
      case '*':
        loop = false;
        break;
      default:
        break;
      }

      {
        uint32_t startAt;
        uint32_t duration;
        readEntry(entries[sortedIndex], startAt, duration);
        displayCustomSetting(sortedIndex, startAt, duration);
      }
    }
}

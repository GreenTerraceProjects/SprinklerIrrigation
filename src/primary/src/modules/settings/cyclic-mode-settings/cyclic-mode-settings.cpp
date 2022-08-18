#include "cyclic-mode-settings.hpp"
#include "cylic-mode-setting/cylic-mode-setting.hpp"
#include "../../inputs/menu/menu.hpp"
#include "../../devices/eeprom/addresses.hpp"
#include <avr/pgmspace.h>

CyclicModeSettings::CyclicModeSettings(CyclicModeSettingsArgs &args)
    : Input(args) {

}

void CyclicModeSettings::initialize() {
    this->showSettings();
}

bool CyclicModeSettings::handleInput(char inputKey) {
    return false;
}

CyclicModeSettingsResult CyclicModeSettings::createResult() {
    return CyclicModeSettingsResult();
}

static const char menu_item_0[] PROGMEM = "Full Duration";
static const char menu_item_1[] PROGMEM = "Duty Duration";
static const char menu_item_2[] PROGMEM = "Start Time";
static const char menu_item_3[] PROGMEM = "Stop Time";
static const char* const menu[] PROGMEM = { 
    menu_item_0,
    menu_item_1,
    menu_item_2,
    menu_item_3
};

void CyclicModeSettings::showSettings() {
    const int menuSize = sizeof(menu) / sizeof(menu[0]);
    MenuArgs menuArgs {
        .items = menu,
        .numItems = menuSize,
        .defaultItemIndex = this->selectedSettingIndex
    };

    Menu menu(menuArgs);
    while (true) {
        menu.run();

        if (menu.isCanceled()) {
            this->cancel();
            return;
        }

        MenuResult menuResult = menu.getResult();
        this->selectedSettingIndex = menuResult.selectedIndex;

        this->showSetting();
    }    
}

void CyclicModeSettings::showSetting() {
    const bool FULL_TIMESPAN_LOOKUP[4] = { true, true, false, false };
    const uint32_t MAX_LOOKUP[4] = { 86400, 86400, 86400 - 1, 86400 - 1 };

    CyclicModeSettingArgs args {
        .address = EEPROM_CYCLIC_MODE_SETTINGS_ADDRESS + sizeof(uint32_t) * this->selectedSettingIndex,
        .isFullTimeSpan = FULL_TIMESPAN_LOOKUP[this->selectedSettingIndex],
        .maxValue = MAX_LOOKUP[this->selectedSettingIndex],
        .title = (const char *)pgm_read_ptr(&menu[this->selectedSettingIndex])
    };
    CyclicModeSetting setting(args);
    setting.run();
}

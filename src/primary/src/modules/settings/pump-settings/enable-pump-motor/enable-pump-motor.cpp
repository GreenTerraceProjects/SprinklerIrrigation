#include "enable-pump-motor.hpp"
#include "../../../inputs/menu/menu.hpp"
#include "../../../devices/eeprom/addresses.hpp"
#include <avr/pgmspace.h>
#include <EEPROM.h>

EnablePumpMotor::EnablePumpMotor()
    : Input() {

}

void EnablePumpMotor::initialize() {
    this->showOptions();
}

bool EnablePumpMotor::handleKeyPressed(char inputKey) {
    return false;
}

bool EnablePumpMotor::readSetting() {
    bool value;
    EEPROM.get(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, value);
    return value;
}

void EnablePumpMotor::writeSetting(bool value) {
    EEPROM.put(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, value);
}

static const char menu_item_0[] PROGMEM = "No";
static const char menu_item_1[] PROGMEM = "Yes";
static const char* const menu[] PROGMEM = {
    menu_item_0,
    menu_item_1
};
void EnablePumpMotor::showOptions() {
    const int numOptions = sizeof(menu) / sizeof(menu[0]);
    MenuArgs args = {
        .items = menu,
        .numItems = numOptions,
        .defaultItemIndex = (int)this->readSetting()
    };
    Menu menu(args);
    menu.run();

    if (menu.isCanceled()) {
        return;
    }

    MenuResult result = menu.getResult();
    this->writeSetting((bool)result.selectedIndex);
}

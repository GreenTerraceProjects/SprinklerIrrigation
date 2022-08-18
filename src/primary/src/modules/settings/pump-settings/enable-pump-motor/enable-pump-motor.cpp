#include "enable-pump-motor.hpp"
#include "enable-pump-motor-reader.hpp"
#include "enable-pump-motor-writer.hpp"
#include "../../../inputs/menu/menu.hpp"
#include <avr/pgmspace.h>

EnablePumpMotor::EnablePumpMotor(EnablePumpMotorArgs &args)
    : Input(args) {

}

void EnablePumpMotor::initialize() {
    this->showOptions();
}

bool EnablePumpMotor::handleInput(char inputKey) {
    return false;
}

EnablePumpMotorResult EnablePumpMotor::createResult() {
    return EnablePumpMotorResult();
}

bool EnablePumpMotor::readSetting() {
    EnablePumpMotorReaderArgs args;
    EnablePumpMotorReader reader(args);
    reader.run();
    EnablePumpMotorReaderResult result = reader.getResult();

    return result.value;
}

void EnablePumpMotor::writeSetting(bool value) {
    EnablePumpMotorWriterArgs args {
        .value = value
    };
    EnablePumpMotorWriter writer(args);
    writer.run();
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

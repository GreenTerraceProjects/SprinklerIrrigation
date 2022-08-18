#include "pump-settings.hpp"
#include "enable-pump-motor/enable-pump-motor.hpp"
#include "water-hammer-delays/water-hammer-delays.hpp"
#include "../../inputs/menu/menu.hpp"
#include <avr/pgmspace.h>

PumpSettings::PumpSettings(PumpSettingsArgs &args)
    : Input(args) {

}

void PumpSettings::initialize() {
    this->showSettings();
}

bool PumpSettings::handleInput(char inputKey) {
    return false;
}

PumpSettingsResult PumpSettings::createResult() {
    return PumpSettingsResult();
}

static const char pump_settings_menu_item_0[] PROGMEM = "Pump Motor Enabled";
static const char pump_settings_menu_Item_1[] PROGMEM = "Water Hammer Delay";
static const char* const pump_settings_menu[] PROGMEM = {
  pump_settings_menu_item_0,
  pump_settings_menu_Item_1
};
void PumpSettings::showSettings() {
    const int menuSize = sizeof(pump_settings_menu) / sizeof(pump_settings_menu[0]);
    MenuArgs args {
        .items = pump_settings_menu,
        .numItems = menuSize,
        .defaultItemIndex = 0
    };
    Menu menu(args);

    while (true) {
        menu.run();

        if (menu.isCanceled()) {
            return;
        }

        MenuResult result = menu.getResult();
        switch (result.selectedIndex) {
            case 0:
                this->showEnablePumpMotor();
                break;
            case 1:
                this->showWaterHammerDelays();
                break;
            default:
                break;
        }

        args.defaultItemIndex = result.selectedIndex;
    }
}

void PumpSettings::showEnablePumpMotor(){
    EnablePumpMotorArgs args;
    EnablePumpMotor enablePumpMotor(args);
    enablePumpMotor.run();
}

void PumpSettings::showWaterHammerDelays(){
    WaterHammerDelaysArgs args;
    WaterHammerDelays waterHammerDelays(args);
    waterHammerDelays.run();
}

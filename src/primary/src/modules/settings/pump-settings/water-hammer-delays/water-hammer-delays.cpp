#include "water-hammer-delays.hpp"
#include "water-hammer-delay/water-hammer-delay.hpp"
#include "../../../inputs/menu/menu.hpp"
#include <avr/pgmspace.h>

WaterHammerDelays::WaterHammerDelays(WaterHammerDelaysArgs &args)
    : Input(args) {

}

void WaterHammerDelays::initialize() {

}

bool WaterHammerDelays::handleInput(char inputKey) {
    return false;
}

WaterHammerDelaysResult WaterHammerDelays::createResult() {
    return WaterHammerDelaysResult();
}

const char water_hammer_delay_when_start_pump[] PROGMEM = "When start pump";
const char water_hammer_delay_when_stop_pump[] PROGMEM = "When stop pump";
const char* const water_hammer_delay_menu[] PROGMEM = {
    water_hammer_delay_when_start_pump,
    water_hammer_delay_when_stop_pump
};

void WaterHammerDelays::showDelays() {
    const int menuSize = sizeof(water_hammer_delay_menu) / sizeof(water_hammer_delay_menu[0]);

    MenuArgs menuArgs {
        .items = water_hammer_delay_menu,
        .numItems = menuSize,
        .defaultItemIndex = this->selectedDelayIndex
    };

    Menu menu(menuArgs);

    while (true) {
        menu.run();

        if (menu.isCanceled()) {
            this->cancel();
            return;
        }

        MenuResult menuResult = menu.getResult();
        this->selectedDelayIndex = menuResult.selectedIndex;
        
        this->showDelay();
    }
}

void WaterHammerDelays::showDelay() {
    WaterHammerDelayArgs args {
        .selectedDelayIndex = this->selectedDelayIndex
    };
    WaterHammerDelay waterHammerDelay(args);
    waterHammerDelay.run();
}

#include "water-hammer-delay.hpp"
#include "../../../../devices/eeprom/addresses.hpp"
#include "../../../../devices/lcd/lcd.hpp"
#include "../../../../devices/buzzer/buzzer.hpp"
#include "../../../../inputs/unsigned-integer-input/unsigned-integer-input.hpp"
#include <EEPROM.h>

WaterHammerDelay::WaterHammerDelay(WaterHammerDelayArgs &args)
    : args(args), Input() {

}

void WaterHammerDelay::initialize() {
    
}

bool WaterHammerDelay::handleKeyPressed(char inputKey) {
    switch (inputKey) {
        case '5':
            this->doInput();
            break;
        case '*':
            return false;
            break;
        default:
            break;
    }

    this->showDelay();
    return true;
}

uint16_t WaterHammerDelay::readDelay() {
    int eepromAddress = EEPROM_PREVENT_WATER_HAMMER_SETTING_ADDRESS + sizeof(uint16_t) * this->args.selectedDelayIndex;

    uint16_t value;
    EEPROM.get(eepromAddress, value);
    return value;
}

void WaterHammerDelay::writeDelay(uint16_t value) {
    int eepromAddress = EEPROM_PREVENT_WATER_HAMMER_SETTING_ADDRESS + sizeof(uint16_t) * this->args.selectedDelayIndex;
    EEPROM.put(eepromAddress, value);
}

void WaterHammerDelay::showDelay() {
    const int OUTPUT_BUFFER_SIZE = LCD_COLS + 1;
    char buffer[OUTPUT_BUFFER_SIZE] = { '\0' };

    snprintf_P(
        buffer,
        OUTPUT_BUFFER_SIZE,
        PSTR("[0; %u] ms"),
        60000);
    lcd.print(buffer);
    lcd.setCursor(0, 1);
    lcd.print(this->readDelay());
}

void WaterHammerDelay::doInput() {
    UnsignedIntegerInputArgs unsignedIntegerInputArgs {
        .defaultValue = this->readDelay()
    };

    UnsignedIntegerInput unsignedIntegerInput(unsignedIntegerInputArgs);
    while (true) {
        unsignedIntegerInput.run();

        if (unsignedIntegerInput.isCanceled()) {
            return;
        }

        uint32_t value = unsignedIntegerInput.getValue();
        if (value > 60000) {
            unsignedIntegerInputArgs.defaultValue = value;
            lowBeep();
            continue;
        }

        this->writeDelay(value);
        break;
    }
}

#include "water-hammer-delay.hpp"
#include "water-hammer-delay-input.hpp"
#include "water-hammer-delay-label.hpp"
#include "water-hammer-delay-reader.hpp"
#include "water-hammer-delay-writer.hpp"
#include "../../../../devices/eeprom/addresses.hpp"

WaterHammerDelay::WaterHammerDelay(WaterHammerDelayArgs &args)
    : Input(args) {

}

void WaterHammerDelay::initialize() {
    
}

bool WaterHammerDelay::handleInput(char inputKey) {
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

WaterHammerDelayResult WaterHammerDelay::createResult() {
    return WaterHammerDelayResult();
}

uint16_t WaterHammerDelay::readDelay() {
    int eepromAddress = EEPROM_PREVENT_WATER_HAMMER_SETTING_ADDRESS + sizeof(uint16_t) * this->args.selectedDelayIndex;
    WaterHammerDelayReaderArgs waterHammerDelayReaderArgs = {
        .address = eepromAddress
    };
    WaterHammerDelayReader waterHammerDelayReader(waterHammerDelayReaderArgs);
    waterHammerDelayReader.run();
    auto waterHammerDelayReaderResult = waterHammerDelayReader.getResult();
    return waterHammerDelayReaderResult.value;
}

void WaterHammerDelay::writeDelay(uint16_t value) {
    int eepromAddress = EEPROM_PREVENT_WATER_HAMMER_SETTING_ADDRESS + sizeof(uint16_t) * this->args.selectedDelayIndex;
    WaterHammerDelayWriterArgs waterHammerDelayWriterArgs = {
        .address = eepromAddress,
        .value = value
    };

    WaterHammerDelayWriter waterHammerDelayWriter(waterHammerDelayWriterArgs);
    waterHammerDelayWriter.run();
}

void WaterHammerDelay::showDelay() {
    WaterHammerDelayLabelArgs args = {
        .value = this->readDelay(),
        .maxValue = 60000
    };

    WaterHammerDelayLabel waterHammerDelayLabel(args);
    waterHammerDelayLabel.view();
}

void WaterHammerDelay::doInput() {
    WaterHammerDelayInputArgs waterHammerDelayInputArgs = {
        .defaultValue = this->readDelay(),
        .maxValue = 60000
    };

    WaterHammerDelayInput waterHammerDelayInput(waterHammerDelayInputArgs);
    waterHammerDelayInput.run();

    if (waterHammerDelayInput.isCanceled()) {
        return;
    }    
    
    WaterHammerDelayInputResult waterHammerDelayInputResult = waterHammerDelayInput.getResult();
    this->writeDelay(waterHammerDelayInputResult.value);
}

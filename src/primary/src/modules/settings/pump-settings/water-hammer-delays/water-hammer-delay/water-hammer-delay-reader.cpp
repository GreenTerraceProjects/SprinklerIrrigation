#include "water-hammer-delay-reader.hpp"
#include <EEPROM.h>

WaterHammerDelayReader::WaterHammerDelayReader(WaterHammerDelayReaderArgs &args)
    : value(0), SettingReader(args) {

}

enum ERROR WaterHammerDelayReader::read() {
    EEPROM.get(this->args.address, this->value);
    return ERROR::NONE;
}

WaterHammerDelayReaderResult WaterHammerDelayReader::createResult() {
    return WaterHammerDelayReaderResult {
        .value = this->value
    };
}

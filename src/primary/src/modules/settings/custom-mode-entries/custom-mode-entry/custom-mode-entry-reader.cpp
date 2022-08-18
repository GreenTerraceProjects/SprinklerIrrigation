#include "custom-mode-entry-reader.hpp"
#include <EEPROM.h>
#include "../../../devices/eeprom/addresses.hpp"

CustomModeEntryReader::CustomModeEntryReader(CustomModeEntryReaderArgs &args)
    : startAt(0), duration(0), SettingReader(args) {

}

enum ERROR CustomModeEntryReader::read() {
    int address = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS
        + sizeof(uint32_t) * 2 * this->args.index;
    EEPROM.get(address, this->startAt);
    
    address += sizeof(uint32_t);
    EEPROM.get(address, this->duration);
    
    return ERROR::NONE;
}

CustomModeEntryReaderResult CustomModeEntryReader::createResult() {
    return CustomModeEntryReaderResult {
        .startAt = this->startAt,
        .duration = this->duration
    };
}

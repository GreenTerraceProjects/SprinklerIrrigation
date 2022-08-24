#include "custom-mode-entry-reader.hpp"
#include <EEPROM.h>
#include "../../../devices/eeprom/addresses.hpp"

CustomModeEntryReader::CustomModeEntryReader(CustomModeEntryReaderArgs &args)
    : args(args) {

}

CustomModeEntryReaderResult CustomModeEntryReader::read() {
    uint32_t startAt;
    uint32_t duration;

    int address = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS
        + sizeof(uint32_t) * 2 * this->args.index;
    EEPROM.get(address, startAt);
    
    address += sizeof(uint32_t);
    EEPROM.get(address, duration);
    
    return CustomModeEntryReaderResult {
        .startAt = startAt,
        .duration = duration
    };
}

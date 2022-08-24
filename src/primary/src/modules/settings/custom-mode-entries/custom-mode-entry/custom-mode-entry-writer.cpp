#include "custom-mode-entry-writer.hpp"
#include "../../../devices/eeprom/addresses.hpp"
#include <EEPROM.h>

CustomModeEntryWriter::CustomModeEntryWriter(CustomModeEntryWriterArgs &args)
    : args(args) {

}

void CustomModeEntryWriter::write() {
    int address = EEPROM_CUSTOM_MODE_SETTINGS_ADDRESS
        + sizeof(uint32_t) * 2 * this->args.index;
    EEPROM.put(address, this->args.startAt);
    
    address += sizeof(uint32_t);
    EEPROM.put(address, this->args.duration);
}

#include <EEPROM.h>
#include "eeprom-cleaning-reader.hpp"
#include <stdint.h>

EEPROMCleaningReader::EEPROMCleaningReader(EEPROMCleaningReaderArgs &args)
    : isClear(false), SettingReader(args) {

}

enum ERROR EEPROMCleaningReader::read() {
    this->isClear = true;

    for (int index = 0; index < EEPROM.length(); ++index) {
        uint8_t value;
        EEPROM.get(index, value);

        if (value != 0) {
            this->isClear = false;
            break;
        }
    }

    return ERROR::NONE;
}

EEPROMCleaningReaderResult EEPROMCleaningReader::createResult() {
    return EEPROMCleaningReaderResult {
        .isClear = this->isClear
    };
}

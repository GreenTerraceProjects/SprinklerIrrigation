#include "cyclic-mode-setting-reader.hpp"
#include <EEPROM.h>

CyclicModeSettingReader::CyclicModeSettingReader(CyclicModeSettingReaderArgs &args)
    : value(0), SettingReader(args) {

}

enum ERROR CyclicModeSettingReader::read() {
    EEPROM.get(this->args.address, this->value);
    return ERROR::NONE;
}

CyclicModeSettingReaderResult CyclicModeSettingReader::createResult() {
    return CyclicModeSettingReaderResult {
        .value = this->value
    };
}

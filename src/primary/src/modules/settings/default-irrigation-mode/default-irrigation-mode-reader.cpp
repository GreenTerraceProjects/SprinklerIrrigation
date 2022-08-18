#include "default-irrigation-mode-reader.hpp"
#include "../../devices/eeprom/addresses.hpp"
#include <EEPROM.h>

DefaultIrrigationModeReader::DefaultIrrigationModeReader(DefaultIrrigationModeReaderArgs &args)
    : SettingReader(args) {

}

enum ERROR DefaultIrrigationModeReader::read() {
    EEPROM.get(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, this->value);
    return ERROR::NONE;
}

DefaultIrrigationModeReaderResult DefaultIrrigationModeReader::createResult() {
    return DefaultIrrigationModeReaderResult {
        .value = this->value
    };
}

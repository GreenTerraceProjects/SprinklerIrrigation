#include "default-irrigation-mode-writer.hpp"
#include "../../devices/eeprom/addresses.hpp"
#include <EEPROM.h>

DefaultIrrigationModeWriter::DefaultIrrigationModeWriter(DefaultIrrigationModeWriterArgs &args)
    : SettingWriter(args) {

}

enum ERROR DefaultIrrigationModeWriter::write() {
    EEPROM.put(EEPROM_DEFAULT_IRRIGATION_MODE_SETTING_ADDRESS, this->args.value);
    return ERROR::NONE;
}

DefaultIrrigationModeWriterResult DefaultIrrigationModeWriter::createResult() {
    return DefaultIrrigationModeWriterResult();
}

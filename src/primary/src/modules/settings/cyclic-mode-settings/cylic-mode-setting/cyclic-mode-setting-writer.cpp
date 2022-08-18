#include "cyclic-mode-setting-writer.hpp"
#include <EEPROM.h>

CyclicModeSettingWriter::CyclicModeSettingWriter(CyclicModeSettingWriterArgs &args)
    : SettingWriter(args) {

}

enum ERROR CyclicModeSettingWriter::write() {
    EEPROM.put(this->args.address, this->args.value);
    return ERROR::NONE;
}

CyclicModeSettingWriterResult CyclicModeSettingWriter::createResult() {
    return CyclicModeSettingWriterResult();
}

#include "eeprom-cleaning-writer.hpp"
#include <EEPROM.h>

EEPROMCleaningWriter::EEPROMCleaningWriter(EEPROMCleaningWriterArgs &args)
    : SettingWriter(args) {

}

enum ERROR EEPROMCleaningWriter::write() {
    for (int index = 0; index < EEPROM.length(); ++index) {
      EEPROM.write(index, 0);
    }
    return ERROR::NONE;
}

EEPROMCleaningWriterResult EEPROMCleaningWriter::createResult() {
    return EEPROMCleaningWriterResult();
}

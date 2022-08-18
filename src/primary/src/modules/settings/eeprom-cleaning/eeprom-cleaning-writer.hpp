#ifndef _EEPROM_CLEANING_WRITER_HPP_
#define _EEPROM_CLEANING_WRITER_HPP_

#include "../setting-writer.hpp"

class EEPROMCleaningWriterArgs {

};

class EEPROMCleaningWriterResult {

};

class EEPROMCleaningWriter : public SettingWriter<EEPROMCleaningWriterResult, EEPROMCleaningWriterArgs> {
public:
    EEPROMCleaningWriter(EEPROMCleaningWriterArgs &args);

protected:
    enum ERROR write() override;
    EEPROMCleaningWriterResult createResult() override;
};

#endif

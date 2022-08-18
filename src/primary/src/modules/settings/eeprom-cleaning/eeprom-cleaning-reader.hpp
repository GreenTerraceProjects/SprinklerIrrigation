#ifndef _EEPROM_CLEANING_READER_HPP_
#define _EEPROM_CLEANING_READER_HPP_

#include "../setting-reader.hpp"

class EEPROMCleaningReaderArgs {

};

class EEPROMCleaningReaderResult {
public:
    bool isClear;
};

class EEPROMCleaningReader : public SettingReader<EEPROMCleaningReaderResult, EEPROMCleaningReaderArgs> {
public:
    EEPROMCleaningReader(EEPROMCleaningReaderArgs &args);

protected:
    bool isClear;

protected:
    enum ERROR read() override;
    EEPROMCleaningReaderResult createResult() override;
};

#endif

#ifndef _EEPROM_EEPROM_HPP_
#define _EEPROM_EEPROM_HPP_

#include "../../inputs/input.hpp"

class EEPROMCleaningArgs {

};

class EEPROMCleaningResult {

};

class EEPROMCleaning : public Input<EEPROMCleaningResult, EEPROMCleaningArgs> {
public:
    EEPROMCleaning(EEPROMCleaningArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    EEPROMCleaningResult createResult() override;
};

#endif

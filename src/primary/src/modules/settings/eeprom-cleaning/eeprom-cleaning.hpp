#ifndef _EEPROM_EEPROM_HPP_
#define _EEPROM_EEPROM_HPP_

#include "../../inputs/input.hpp"

class EEPROMCleaning : public Input {
public:
    EEPROMCleaning();

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

private:
    bool confirmCleaning();
    void informCleaned();
    void clean();
};

#endif

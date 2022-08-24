#ifndef _DEFAULT_IRRIGATION_MODE_HPP_
#define _DEFAULT_IRRIGATION_MODE_HPP_

#include "../../inputs/input.hpp"
#include <stdint.h>

class DefaultIrrigationMode : public Input {
public:
    DefaultIrrigationMode();

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    uint8_t readDefaultIrrigationMode();
    void writeDefaultIrriationMode(uint8_t value);
};

#endif

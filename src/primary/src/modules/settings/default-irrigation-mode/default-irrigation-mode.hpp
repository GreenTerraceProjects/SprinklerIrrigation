#ifndef _DEFAULT_IRRIGATION_MODE_HPP_
#define _DEFAULT_IRRIGATION_MODE_HPP_

#include "../../inputs/input.hpp"
#include <stdint.h>

class DefaultIrrigationModeArgs {

};

class DefaultIrrigationModeResult {

};

class DefaultIrrigationMode : public Input<DefaultIrrigationModeResult, DefaultIrrigationModeArgs> {
public:
    DefaultIrrigationMode(DefaultIrrigationModeArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    DefaultIrrigationModeResult createResult() override;

protected:
    uint8_t readDefaultIrrigationMode();
    void writeDefaultIrriationMode(uint8_t value);
};

#endif

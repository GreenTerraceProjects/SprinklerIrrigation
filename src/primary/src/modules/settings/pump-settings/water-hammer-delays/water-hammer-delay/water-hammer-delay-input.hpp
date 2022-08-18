#ifndef _WATER_HAMMER_DELAY_INPUT_HPP_
#define _WATER_HAMMER_DELAY_INPUT_HPP_

#include "../../../../inputs/input.hpp"
#include <stdint.h>

class WaterHammerDelayInputArgs {
public:
    uint16_t defaultValue;
    uint16_t maxValue;
};

class WaterHammerDelayInputResult {
public:
    uint16_t value;
};

class WaterHammerDelayInput : public Input<WaterHammerDelayInputResult, WaterHammerDelayInputArgs> {
public:
    WaterHammerDelayInput(WaterHammerDelayInputArgs &args);

protected:
    uint16_t value;

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    WaterHammerDelayInputResult createResult() override;

protected:
    void doInputUnsignedInteger();
};

#endif

#ifndef _WATER_HAMMER_DELAY_HPP_
#define _WATER_HAMMER_DELAY_HPP_

#include "../../../../inputs/input.hpp"
#include <stdint.h>

class WaterHammerDelayArgs {
public:
    int selectedDelayIndex;
};

class WaterHammerDelayResult {

};

class WaterHammerDelay : public Input<WaterHammerDelayResult, WaterHammerDelayArgs> {
public:
    WaterHammerDelay(WaterHammerDelayArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    WaterHammerDelayResult createResult() override;

protected:
    uint16_t readDelay();
    void writeDelay(uint16_t value);

protected:
    void showDelay();
    void doInput();
};

#endif

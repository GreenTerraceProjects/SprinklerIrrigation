#ifndef _WATER_HAMMER_DELAYS_HPP_
#define _WATER_HAMMER_DELAYS_HPP_

#include "../../../inputs/input.hpp"

class WaterHammerDelaysArgs {

};

class WaterHammerDelaysResult {

};

class WaterHammerDelays : public Input<WaterHammerDelaysResult, WaterHammerDelaysArgs> {
public:
    WaterHammerDelays(WaterHammerDelaysArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    WaterHammerDelaysResult createResult() override;

protected:
    int selectedDelayIndex;

protected:
    void showDelays();
    void showDelay();
};

#endif

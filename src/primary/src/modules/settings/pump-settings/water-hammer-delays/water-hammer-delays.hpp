#ifndef _WATER_HAMMER_DELAYS_HPP_
#define _WATER_HAMMER_DELAYS_HPP_

#include "../../../inputs/input.hpp"

class WaterHammerDelays : public Input {
public:
    WaterHammerDelays();

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    int selectedDelayIndex;

protected:
    void showDelays();
    void showDelay();
};

#endif

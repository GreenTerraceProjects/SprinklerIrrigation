#ifndef _WATER_HAMMER_DELAY_LABEL_HPP_
#define _WATER_HAMMER_DELAY_LABEL_HPP_

#include "../../../../labels/label.hpp"
#include <stdint.h>

class WaterHammerDelayLabelArgs {
public:
    uint16_t value;
    uint16_t maxValue;
};

class WaterHammerDelayLabel : public Label<WaterHammerDelayLabelArgs> {
public:
    WaterHammerDelayLabel(WaterHammerDelayLabelArgs &args);

protected:
    void initialize() override;
    void display() override;
};

#endif

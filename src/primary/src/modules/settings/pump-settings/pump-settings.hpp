#ifndef _PUMP_SETTINGS_HPP_
#define _PUMP_SETTINGS_HPP_

#include "../../inputs/input.hpp"

class PumpSettings : public Input {
public:
    PumpSettings();

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    void showSettings();
    void showEnablePumpMotor();
    void showWaterHammerDelays();
};

#endif

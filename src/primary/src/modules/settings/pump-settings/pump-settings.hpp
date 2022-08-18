#ifndef _PUMP_SETTINGS_HPP_
#define _PUMP_SETTINGS_HPP_

#include "../../inputs/input.hpp"

class PumpSettingsArgs {

};

class PumpSettingsResult {

};

class PumpSettings : public Input<PumpSettingsResult, PumpSettingsArgs> {
public:
    PumpSettings(PumpSettingsArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    PumpSettingsResult createResult() override;

protected:
    void showSettings();
    void showEnablePumpMotor();
    void showWaterHammerDelays();
};

#endif

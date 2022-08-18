#ifndef _CYCLIC_MODE_SETTINGS_HPP_
#define _CYCLIC_MODE_SETTINGS_HPP_

#include "../../inputs/input.hpp"

class CyclicModeSettingsArgs {

};

class CyclicModeSettingsResult {

};

class CyclicModeSettings : public Input<CyclicModeSettingsResult, CyclicModeSettingsArgs> {
public:
    CyclicModeSettings(CyclicModeSettingsArgs &args);

protected:
    int selectedSettingIndex;

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    CyclicModeSettingsResult createResult() override;

protected:
    void showSettings();
    void showSetting();
};

#endif

#ifndef _CYCLIC_MODE_SETTINGS_HPP_
#define _CYCLIC_MODE_SETTINGS_HPP_

#include "../../inputs/input.hpp"

class CyclicModeSettings : public Input {
public:
    CyclicModeSettings();

protected:
    int selectedSettingIndex;

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    void showSettings();
    void showSetting();
};

#endif

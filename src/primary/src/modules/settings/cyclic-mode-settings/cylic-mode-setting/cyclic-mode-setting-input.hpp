#ifndef _CYCLIC_MODE_SETTING_INPUT_HPP_
#define _CYCLIC_MODE_SETTING_INPUT_HPP_

#include "../../../inputs/input.hpp"
#include <stdint.h>

class CylicModeSettingInputArgs {
public:
    uint32_t defaultValue;
    bool isFullTimeSpan;
    uint32_t maxValue;
};

class CylicModeSettingInputResult {
public:
    uint32_t value;
};

class CylicModeSettingInput : public Input<CylicModeSettingInputResult, CylicModeSettingInputArgs> {
public:
    CylicModeSettingInput(CylicModeSettingInputArgs &args);

protected:
    uint32_t value;

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    CylicModeSettingInputResult createResult() override;

protected:
    void doInput();
};

#endif

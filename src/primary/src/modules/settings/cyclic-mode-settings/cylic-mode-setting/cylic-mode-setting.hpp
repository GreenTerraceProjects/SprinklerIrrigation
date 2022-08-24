#ifndef _CYCLIC_MODE_SETTINNG_HPP_
#define _CYCLIC_MODE_SETTINNG_HPP_

#include "../../../inputs/input.hpp"
#include <stdint.h>

class CyclicModeSettingArgs {
public:
    int address;
    bool isFullTimeSpan;
    uint32_t maxValue;
    const char *title;
};

class CyclicModeSetting : public Input {
public:
    CyclicModeSetting(CyclicModeSettingArgs &args);

protected:
    CyclicModeSettingArgs &args;

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    uint32_t readSetting();
    void writeSetting(uint32_t value);
    void showSetting();
    void inputSetting();
};

#endif

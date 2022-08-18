#ifndef _CYCLIC_MODE_SETTING_LABEL_HPP_
#define _CYCLIC_MODE_SETTING_LABEL_HPP_

#include "../../../labels/label.hpp"
#include <stdint.h>

class CyclicModeSettingLabelArgs {
public:
    uint32_t value;
    bool isFullTimeSpan;    
    const char *title;
};

class CyclicModeSettingLabel : public Label<CyclicModeSettingLabelArgs> {
public:
    CyclicModeSettingLabel(CyclicModeSettingLabelArgs &args);

protected:
    void initialize() override;
    void display() override;
};

#endif

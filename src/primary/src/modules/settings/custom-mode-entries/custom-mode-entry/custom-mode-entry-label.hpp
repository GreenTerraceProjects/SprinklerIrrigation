#ifndef _CUSTOM_MODE_ENTRY_LABEL_HPP_
#define _CUSTOM_MODE_ENTRY_LABEL_HPP_

#include "../../../labels/label.hpp"
#include "RTClib.h"

class CustomModeEntryLabelArgs {
public:
    int index;
    uint32_t startAt;
    uint32_t duration;
};

class CustomModeEntryLabel: public Label {
public:
    CustomModeEntryLabel(CustomModeEntryLabelArgs &args);

protected:
    CustomModeEntryLabelArgs &args;

protected:
    void initialize() override;
    void display() override;
};

#endif

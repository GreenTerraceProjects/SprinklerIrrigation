#ifndef _CUSTOM_MODE_ENTRY_INPUT_HPP_
#define _CUSTOM_MODE_ENTRY_INPUT_HPP_

#include "../../../inputs/input.hpp"

class CustomModeEntryInputArgs {
public:
    int index;
    uint32_t defaultStartAt;
    uint32_t defaultDuration;
};

class CustomModeEntryInputResult {
public:
    uint32_t startAt;
    uint32_t duration;
};

class CustomModeEntryInput
    : public Input<
        CustomModeEntryInputResult,
        CustomModeEntryInputArgs> {
public:
    CustomModeEntryInput(CustomModeEntryInputArgs &args);

protected:
    int lineIndex;
    uint32_t startAt;
    uint32_t duration;

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    CustomModeEntryInputResult createResult() override;

protected:
    void viewEntry();
    void inputTimeSpan();
};

#endif

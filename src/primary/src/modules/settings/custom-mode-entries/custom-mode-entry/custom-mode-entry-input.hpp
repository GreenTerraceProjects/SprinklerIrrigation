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
    : public Input {
public:
    CustomModeEntryInput(CustomModeEntryInputArgs &args);

protected:
    CustomModeEntryInputArgs &args;

protected:
    int lineIndex;
    uint32_t startAt;
    uint32_t duration;

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

public:
    CustomModeEntryInputResult getResult();

protected:
    void viewEntry();
    void inputTimeSpan();
};

#endif

#ifndef _UNSIGNED_INTEGER_INPUT_HPP_
#define _UNSIGNED_INTEGER_INPUT_HPP_

#include "../input.hpp"
#include <stdint.h>

class UnsignedIntegerInputArgs {
public:
    uint32_t defaultValue;
};

class UnsignedIntegerInputResult {
public:
    uint32_t value;
};

class UnsignedIntegerInput : public Input<UnsignedIntegerInputResult, UnsignedIntegerInputArgs> {
public:
    UnsignedIntegerInput(UnsignedIntegerInputArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    UnsignedIntegerInputResult createResult() override;

protected:
    static const int NUMBER_LENGTH = 10;
    static const int INPUT_BUFFER_LENGTH = NUMBER_LENGTH + 1;
    char inputBuffer[INPUT_BUFFER_LENGTH];
    int index;

protected:
    void refreshDisplay();
};

uint32_t inputUnsignedInteger(uint32_t defaultValue, bool* isCanceled);

#endif

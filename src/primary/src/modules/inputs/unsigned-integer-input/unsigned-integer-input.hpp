#ifndef _UNSIGNED_INTEGER_INPUT_HPP_
#define _UNSIGNED_INTEGER_INPUT_HPP_

#include "../input.hpp"
#include <stdint.h>

class UnsignedIntegerInputArgs {
public:
    uint32_t defaultValue;
};

class UnsignedIntegerInput : public Input {
public:
    UnsignedIntegerInput(UnsignedIntegerInputArgs &args);

protected:
    UnsignedIntegerInputArgs &args;

public:
    uint32_t getValue();

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    static const int NUMBER_LENGTH = 10;
    static const int INPUT_BUFFER_LENGTH = NUMBER_LENGTH + 1;
    char inputBuffer[INPUT_BUFFER_LENGTH];
    int index;

protected:
    void refreshDisplay();
};

#endif

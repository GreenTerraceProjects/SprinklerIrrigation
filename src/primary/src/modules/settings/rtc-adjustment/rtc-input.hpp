#ifndef _RTC_INPUT_HPP_
#define _RTC_INPUT_HPP_

#include <RTClib.h>
#include "../../inputs/input.hpp"

class RtcInputArgs {
public:
    DateTime defaultValue;
};

class RtcInputResult {
public:
    DateTime value;
};

class RtcInput : public Input<RtcInputResult, RtcInputArgs> {
public:
    RtcInput(RtcInputArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    RtcInputResult createResult() override;
};

#endif

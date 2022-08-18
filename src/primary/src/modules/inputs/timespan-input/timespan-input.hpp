#ifndef _TIMESPAN_INPUT_HPP_
#define _TIMESPAN_INPUT_HPP_

#include <RTClib.h>
#include "../input.hpp"

class TimeSpanInputArgs {
public:
    bool isFullTimeSpan;
    TimeSpan defaultValue;
};

class TimeSpanInputResult {
public:
    TimeSpan value;
};

class TimeSpanInput : public Input<TimeSpanInputResult, TimeSpanInputArgs> {
public:
    TimeSpanInput(TimeSpanInputArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    TimeSpanInputResult createResult() override;

protected:
    static const int CUSOR_PADDING = 4;
    static const int INPUT_BUFFER_LENGTH = 12;

protected:
    char inputBuffer[INPUT_BUFFER_LENGTH];
    int index;
    int days, hours, minutes, seconds;

protected:
    void setupDisplayFrame();
    void refreshDisplay();
};

TimeSpan inputTimeSpan(bool fullTimeSpan, TimeSpan *defaultValue, bool *isCanceled);
TimeSpan inputFullTimeSpan(TimeSpan *defaultValue, bool *isCanceled);
TimeSpan inputShortTimeSpan(TimeSpan *defaultValue, bool *isCanceled);

#endif

#ifndef _TIMESPAN_INPUT_HPP_
#define _TIMESPAN_INPUT_HPP_

#include <RTClib.h>
#include "../input.hpp"

class TimeSpanInputArgs {
public:
    bool isFullTimeSpan;
    TimeSpan defaultValue;
};

class TimeSpanInput : public Input {
public:
    TimeSpanInput(TimeSpanInputArgs &args);

public:
    TimeSpan getTimeSpan();

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    static const int CUSOR_PADDING = 4;
    static const int INPUT_BUFFER_LENGTH = 12;

protected:
    TimeSpanInputArgs &args;
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

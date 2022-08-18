#ifndef _INPUT_DATE_TIME_HPP_
#define _INPUT_DATE_TIME_HPP_

#include <RTClib.h>
#include "../input.hpp"

class DateTimeInputArgs {
public:
    DateTime defaultValue;
};

class DateTimeInputResult {
public:
    DateTime value;
};

class DateTimeInput : public Input<DateTimeInputResult, DateTimeInputArgs> {

public:
    DateTimeInput(DateTimeInputArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    DateTimeInputResult createResult() override;

protected:
    static const int8_t CUSOR_PADDING = 6;
    static const int INPUT_BUFFER_LENGTH = 12;

protected:
    char inputBuffer0[INPUT_BUFFER_LENGTH];
    char inputBuffer1[INPUT_BUFFER_LENGTH];
    int lineIndex;
    int index0;
    int index1;
    int year, month, day, hour, minute, second;

protected:
    void setupDisplayFrame();
    void refreshDisplay();
};

DateTime inputDateTime(DateTime* defaultValue);

#endif

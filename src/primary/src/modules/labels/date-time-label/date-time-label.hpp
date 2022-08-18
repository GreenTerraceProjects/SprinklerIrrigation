#ifndef __DATE_TIME_LABEL_HPP_
#define __DATE_TIME_LABEL_HPP_

#include "../label.hpp"
#include <RTClib.h>

class DateTimeLabelArgs {
public:
    DateTimeLabelArgs(DateTime value);

public:
    DateTime value;
};

class DateTimeLabel : public Label<DateTimeLabelArgs> {
public:
    DateTimeLabel(DateTimeLabelArgs &args);

protected:
    void initialize() override;
    void display() override;
};

#endif

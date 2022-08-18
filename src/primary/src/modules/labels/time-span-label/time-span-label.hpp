#ifndef _TIME_SPAN_LABEL_HPP_
#define _TIME_SPAN_LABEL_HPP_

#include "../label.hpp"
#include "RTClib.h"

class TimeSpanLabelArgs {
public:
    TimeSpan value;
    bool isFullTimeSpan;
    const char *label;
};

class TimeSpanLabel : public Label<TimeSpanLabelArgs> {
public:
    TimeSpanLabel(TimeSpanLabelArgs &args);

protected:
    void initialize() override;
    void display() override;
};

#endif

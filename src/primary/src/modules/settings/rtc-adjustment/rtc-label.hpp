#ifndef _RTC_LABEL_HPP_
#define _RTC_LABEL_HPP_

#include <RTClib.h>
#include "../../labels/label.hpp"
#include "../../labels/date-time-label/date-time-label.hpp"

class RtcLabelArgs : public DateTimeLabelArgs {
public:
    RtcLabelArgs(DateTime value);
};

class RtcLabel : public Label<RtcLabelArgs> {
public:
    RtcLabel(RtcLabelArgs &args);

protected:
    void initialize() override;
    void display() override;
};

#endif

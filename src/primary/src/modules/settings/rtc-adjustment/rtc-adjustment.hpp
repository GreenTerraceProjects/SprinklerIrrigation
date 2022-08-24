#ifndef _RTC_SETTING_HPP_
#define _RTC_SETTING_HPP_

#include "../../inputs/input.hpp"
#include <RTClib.h>

class RtcAdjustment : public Input {
public:
    RtcAdjustment();

protected:
    DateTime lastPolling;

protected:
    void initialize() override;
    
    bool poll() override;
    bool handlePolling() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    DateTime readRtcValue();
    void writeRtcValue(DateTime value);

protected:
    void doRtcLabel();
    void doRtcInput();
};

#endif

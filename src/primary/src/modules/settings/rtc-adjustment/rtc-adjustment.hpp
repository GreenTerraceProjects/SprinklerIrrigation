#ifndef _RTC_SETTING_HPP_
#define _RTC_SETTING_HPP_

#include "../../inputs/input-with-polling.hpp"

class RtcAdjustmentArgs {

};

class RtcAdjustmentResult {

};

class RtcAdjustment : public InputWithPolling<RtcAdjustmentResult, RtcAdjustmentArgs> {
public:
    RtcAdjustment(RtcAdjustmentArgs &args);

protected:
    DateTime lastPolling;

protected:
    void initialize() override;
    
    bool poll() override;
    bool handlePolling() override;
    bool handleInput(char inputKey) override;
    
    RtcAdjustmentResult createResult() override;

protected:
    DateTime readRtcValue();
    void writeRtcValue(DateTime value);

protected:
    void doRtcLabel();
    void doRtcInput();
};

#endif

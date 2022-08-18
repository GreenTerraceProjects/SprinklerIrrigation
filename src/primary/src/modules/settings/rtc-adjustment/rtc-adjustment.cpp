#include "rtc-adjustment.hpp"
#include "rtc-reader.hpp"
#include "rtc-writer.hpp"
#include "rtc-label.hpp"
#include "rtc-input.hpp"
#include "../../devices/rtc/rtc.hpp"

RtcAdjustment::RtcAdjustment(RtcAdjustmentArgs &args) : InputWithPolling(args) {

}

void RtcAdjustment::initialize() {
    this->lastPolling = rtc.now();
}
    
bool RtcAdjustment::poll() {
    DateTime now = rtc.now();
    if (this->lastPolling != now) {
        this->lastPolling = now;
        return true;
    }

    return false;
}

bool RtcAdjustment::handlePolling() {
    this->doRtcLabel();
    return true;
}

bool RtcAdjustment::handleInput(char inputKey) {
    switch (inputKey) {
        case '5':
            this->doRtcInput();
            break;
        case '*':
            return false;
            break;
        default:
            break;
    }

    return true;
}

RtcAdjustmentResult RtcAdjustment::createResult() {
    return RtcAdjustmentResult();
}

DateTime RtcAdjustment::readRtcValue() {
    RtcReaderArgs rtcReaderArgs;
    RtcReader rtcReader(rtcReaderArgs);
    rtcReader.run();
    RtcReaderResult rtcReaderResult = rtcReader.getResult();

    return rtcReaderResult.value;
}

void RtcAdjustment::writeRtcValue(DateTime value) {
    RtcWriterArgs rtcWriterArgs {
        .value = value
    };
    RtcWriter rtcWriter(rtcWriterArgs);
    rtcWriter.run();
}

void RtcAdjustment::doRtcLabel() {
    RtcLabelArgs rtcLabelArgs(this->readRtcValue());
    RtcLabel rtcLabel(rtcLabelArgs);
    rtcLabel.view();
}

void RtcAdjustment::doRtcInput() {
    RtcInputArgs rtcInputArgs {
        .defaultValue = this->readRtcValue()
    };
    RtcInput rtcInput(rtcInputArgs);
    rtcInput.run();

    if (!rtcInput.isCanceled()) {
        RtcInputResult rtcInputResult = rtcInput.getResult();

        this->writeRtcValue(rtcInputResult.value);

        // resetLastInternalRTCUpdate();
        // resetLastInteractTimer();
    }
}

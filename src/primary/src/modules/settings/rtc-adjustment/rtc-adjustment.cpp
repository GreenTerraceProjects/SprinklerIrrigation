#include "rtc-adjustment.hpp"
#include "rtc-reader.hpp"
#include "rtc-writer.hpp"
#include "../../devices/rtc/rtc.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../../inputs/date-time-input/date-time-input.hpp"
#include "../../labels/date-time-label/date-time-label.hpp"

RtcAdjustment::RtcAdjustment() : Input(&keypad) {

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

bool RtcAdjustment::handleKeyPressed(char inputKey) {
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

DateTime RtcAdjustment::readRtcValue() {
    RtcReader rtcReader;
    return rtcReader.read();
}

void RtcAdjustment::writeRtcValue(DateTime value) {
    RtcWriter rtcWriter;
    rtcWriter.write(value);
}

void RtcAdjustment::doRtcLabel() {
    DateTimeLabelArgs args {
        .value = this->readRtcValue()
    };
    DateTimeLabel dateTimeLabel(args);
    dateTimeLabel.view();
}

void RtcAdjustment::doRtcInput() {
    DateTimeInputArgs dateTimeInputArgs {
        .defaultValue = this->readRtcValue()
    };
    DateTimeInput dateTimeInput(dateTimeInputArgs);
    dateTimeInput.run();

    if (!dateTimeInput.isCanceled()) {
        this->writeRtcValue(dateTimeInput.getDateTime());

        // resetLastInternalRTCUpdate();
        // resetLastInteractTimer();
    }
}

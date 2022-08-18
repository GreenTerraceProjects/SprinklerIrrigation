#include "rtc-label.hpp"
#include "../../devices/rtc/rtc.hpp"
#include "../../devices/lcd/lcd.hpp"

RtcLabelArgs::RtcLabelArgs(DateTime value) : DateTimeLabelArgs(value) {

}

RtcLabel::RtcLabel(RtcLabelArgs &args)
  : Label(args) {

}

void RtcLabel::initialize() {

}

void RtcLabel::display() {
  DateTimeLabel dateTimeLabel(this->args);
  dateTimeLabel.view();
}

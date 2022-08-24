#include "rtc-writer.hpp"
#include "../../devices/rtc/rtc.hpp"

void RtcWriter::write(DateTime value) {
    rtc.adjust(value);
    rtcDS3231.adjust(rtc.now());
}

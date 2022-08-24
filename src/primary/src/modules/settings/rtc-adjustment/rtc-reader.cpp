#include "rtc-reader.hpp"
#include "../../devices/rtc/rtc.hpp"

DateTime RtcReader::read() {
    rtc.adjust(rtcDS3231.now());
    return rtc.now();
}

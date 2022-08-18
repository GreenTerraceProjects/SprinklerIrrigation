#include "rtc-reader.hpp"
#include "../../devices/rtc/rtc.hpp"

RtcReader::RtcReader(RtcReaderArgs &args) : SettingReader(args) {

}

enum ERROR RtcReader::read() {
    rtc.adjust(rtcDS3231.now());
    this->value = rtc.now();
    
    return ERROR::NONE;
}

RtcReaderResult RtcReader::createResult() {
    return RtcReaderResult {
        .value = this->value
    };
}

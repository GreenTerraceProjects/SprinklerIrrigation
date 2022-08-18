#include "rtc-writer.hpp"
#include "../../devices/rtc/rtc.hpp"

RtcWriter::RtcWriter(RtcWriterArgs &args) : SettingWriter(args) {

}

enum ERROR RtcWriter::write() {
    rtc.adjust(this->args.value);
    rtcDS3231.adjust(rtc.now());

    return ERROR::NONE;
}

RtcWriterResult RtcWriter::createResult() {
    return RtcWriterResult();
}

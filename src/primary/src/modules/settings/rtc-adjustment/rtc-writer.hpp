#ifndef _RTC_WRITER_HPP_
#define _RTC_WRITER_HPP_

#include <RTClib.h>
#include "../setting-writer.hpp"

class RtcWriterArgs {
public:
    DateTime value;
};

class RtcWriterResult {

};

class RtcWriter : public SettingWriter<RtcWriterResult, RtcWriterArgs> {
public:
    RtcWriter(RtcWriterArgs &args);

protected:
    enum ERROR write() override;
    RtcWriterResult createResult() override;
};

#endif

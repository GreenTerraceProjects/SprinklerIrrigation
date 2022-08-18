#ifndef _RTC_READER_HPP_
#define _RTC_READER_HPP_

#include <RTClib.h>
#include "../setting-reader.hpp"

class RtcReaderArgs {

};

class RtcReaderResult {
public:
    DateTime value;
};

class RtcReader : public SettingReader<RtcReaderResult, RtcReaderArgs> {
public:
    RtcReader(RtcReaderArgs &args);

protected:
    DateTime value;

protected:
    enum ERROR read() override;
    RtcReaderResult createResult() override;
};

#endif

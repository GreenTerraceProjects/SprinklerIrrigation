#ifndef _RTC_READER_HPP_
#define _RTC_READER_HPP_

#include <RTClib.h>

class RtcReader {
public:
    DateTime read();
};

#endif

#ifndef _TIMESPAN_INPUT_HPP_
#define _TIMESPAN_INPUT_HPP_

#include <RTClib.h>

TimeSpan inputFullTimeSpan(TimeSpan *defaultValue, bool *isCanceled);
TimeSpan inputShortTimeSpan(TimeSpan *defaultValue, bool *isCanceled);
TimeSpan inputTimeSpan(bool fullTimeSpan, TimeSpan *defaultValue, bool *isCanceled);

#endif

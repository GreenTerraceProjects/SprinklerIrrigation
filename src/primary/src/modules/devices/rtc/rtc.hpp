#ifndef _RTC_HPP_
#define _RTC_HPP_

#include <RTClib.h>

extern RTC_DS3231 rtcDS3231;
extern RTC_Micros rtc;
extern DateTime lastUpdateInternalRTC;

void setupRTC();
void keepInternalRTCUpToDate();

void resetLastInternalRTCUpdate();
bool doesInternalRTCNeedUpdateFromRTCModule();

#endif

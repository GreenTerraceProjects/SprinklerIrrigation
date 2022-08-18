#include "rtc.hpp"

RTC_DS3231 rtcDS3231;
RTC_Micros rtc;

DateTime lastUpdateInternalRTC;

void setupRTC() {
  while (!rtcDS3231.begin()) {
    delay(100);
  }

  if (rtcDS3231.lostPower()) {
    rtcDS3231.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtcDS3231.clearAlarm(1);
  rtcDS3231.clearAlarm(2);
  rtcDS3231.disableAlarm(1);
  rtcDS3231.disableAlarm(2);
  rtcDS3231.writeSqwPinMode(DS3231_OFF);
  rtcDS3231.disable32K();
  
  rtc.begin(rtcDS3231.now());

  resetLastInternalRTCUpdate();
}

void keepInternalRTCUpToDate() {
  if (doesInternalRTCNeedUpdateFromRTCModule()) {
    rtc.adjust(rtcDS3231.now());
    resetLastInternalRTCUpdate();
  }
}

void resetLastInternalRTCUpdate() {
  lastUpdateInternalRTC = rtc.now();
}

bool doesInternalRTCNeedUpdateFromRTCModule() {
  if ((rtc.now() - lastUpdateInternalRTC).totalseconds() > 3600) {
    return true;
  }

  return false;
}

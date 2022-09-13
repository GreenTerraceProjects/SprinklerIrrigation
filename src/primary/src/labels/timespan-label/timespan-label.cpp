#include "timespan-label.hpp"
#include <avr/pgmspace.h>
#include <stdio.h>
#include "../../devices/lcd/lcd-shadow.hpp"
#include "../../strings/formats/datetime-formats.hpp"

void displayTimeSpan(bool isFullTimeSpan, TimeSpan* timeSpan, const char *label) {
  lcdShadow.clear();

  lcdShadow.print((const __FlashStringHelper *)(&label[0]));

  const int bufferLength = 12;
  char buffer[bufferLength] = { };

  if (isFullTimeSpan) {
    snprintf_P(buffer, bufferLength, (const char *)(&timeSpanFormat[0]),
      timeSpan->days(),
      timeSpan->hours(),
      timeSpan->minutes(),
      timeSpan->seconds());
  } else {
    snprintf_P(buffer, bufferLength, (const char *)(&timeFormat[0]),
      timeSpan->hours(),
      timeSpan->minutes(),
      timeSpan->seconds());
  }

  lcdShadow.setCursor(0, 1);
  lcdShadow.print(buffer);
  lcdShadow.commit();
}

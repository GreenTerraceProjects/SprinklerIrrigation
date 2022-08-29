#include "timespan-label.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../strings/formats/datetime-formats.hpp"

void displayTimeSpan(bool isFullTimeSpan, TimeSpan* timeSpan, const char *label) {
  restoreLcd();

  lcd.print((const __FlashStringHelper *)(&label[0]));

  const int bufferLength = 12;
  char buffer[bufferLength] = { '\0' };

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

  lcd.setCursor(0, 1);
  lcd.print(buffer);
}

#include "datetime-label.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/lcd/lcd-shadow.hpp"
#include "../../strings/formats/datetime-formats.hpp"
#include "../../strings/labels/datetime-labels.hpp"

void displayDateTime(DateTime* dateTime) {
  lcdShadow.clear();

  /*
      ________________
    |Date: 9999-12-31|
    |Time: 23:59:59  |
      ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  */
  const int bufferLength = 12;
  char output[bufferLength] = { };

  lcdShadow.print((const __FlashStringHelper *)(&dateLabel[0]));
  snprintf_P(output, bufferLength, (const char *)(&dateFormat[0]),
    dateTime->year(),
    dateTime->month(),
    dateTime->day());
  lcdShadow.print(output);

  lcdShadow.setCursor(0, 1);
  lcdShadow.print((const __FlashStringHelper *)(&timeLabel[0]));
  snprintf_P(output, bufferLength, (const char *)(&timeFormat[0]),
    dateTime->hour(),
    dateTime->minute(),
    dateTime->second());
  lcdShadow.print(output);

  lcdShadow.commit();
}

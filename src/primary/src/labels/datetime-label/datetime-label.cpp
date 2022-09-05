#include "datetime-label.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../strings/formats/datetime-formats.hpp"
#include "../../strings/labels/datetime-labels.hpp"

void displayDateTime(DateTime* dateTime) {
  restoreLcd();

  /*
      ________________
    |Date: 9999-12-31|
    |Time: 23:59:59  |
      ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  */
  const int bufferLength = 12;
  char output[bufferLength] = { };

  lcd.print((const __FlashStringHelper *)(&dateLabel[0]));
  snprintf_P(output, bufferLength, (const char *)(&dateFormat[0]),
    dateTime->year(),
    dateTime->month(),
    dateTime->day());
  lcd.print(output);

  lcd.setCursor(0, 1);
  lcd.print((const __FlashStringHelper *)(&timeLabel[0]));
  snprintf_P(output, bufferLength, (const char *)(&timeFormat[0]),
    dateTime->hour(),
    dateTime->minute(),
    dateTime->second());
  lcd.print(output);
}

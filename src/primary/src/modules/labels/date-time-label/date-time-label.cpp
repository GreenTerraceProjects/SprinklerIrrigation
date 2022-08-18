#include "date-time-label.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../strings/labels/date-time-labels.hpp"
#include "../../strings/formats/date-time-format-strings.hpp"

DateTimeLabelArgs::DateTimeLabelArgs(DateTime value) : value(value) {

}

DateTimeLabel::DateTimeLabel(DateTimeLabelArgs &args)
    : Label(args) {

}

/*
      ________________
    |Date: 9999-12-31|
    |Time: 23:59:59  |
      ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
*/

void DateTimeLabel::initialize() {
    restoreLcd();
}

void DateTimeLabel::display() {
    const int OUTPUT_BUFFER_LENGTH = 12;
    char buffer[OUTPUT_BUFFER_LENGTH] = { '\0' };

    lcd.print((const __FlashStringHelper *)(&dateLabel[0]));
    snprintf_P(buffer, OUTPUT_BUFFER_LENGTH, (const char *)(&dateFormat[0]),
        this->args.value.year(),
        this->args.value.month(),
        this->args.value.day());
    lcd.print(buffer);

    lcd.setCursor(0, 1);
    lcd.print((const __FlashStringHelper *)(&timeLabel[0]));
    snprintf_P(buffer, OUTPUT_BUFFER_LENGTH, (const char *)(&timeFormat[0]),
        this->args.value.hour(),
        this->args.value.minute(),
        this->args.value.second());
    lcd.print(buffer);
}

#include "time-span-label.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../strings/formats/date-time-format-strings.hpp"

TimeSpanLabel::TimeSpanLabel(TimeSpanLabelArgs &args)
    : Label(args) {

}

void TimeSpanLabel::initialize() {
    restoreLcd();
}

void TimeSpanLabel::display() {
    const int OUTPUT_BUFFER_LENGTH = 12;
    char buffer[OUTPUT_BUFFER_LENGTH];

    lcd.print((const __FlashStringHelper *)(&this->args.label[0]));

    if (this->args.isFullTimeSpan) {
    snprintf_P(buffer, OUTPUT_BUFFER_LENGTH, (const char *)(&timeSpanFormat[0]),
        this->args.value.days(),
        this->args.value.hours(),
        this->args.value.minutes(),
        this->args.value.seconds());
    } else {
    snprintf_P(buffer, OUTPUT_BUFFER_LENGTH, (const char *)(&timeFormat[0]),
        this->args.value.hours(),
        this->args.value.minutes(),
        this->args.value.seconds());
    }

    lcd.setCursor(0, 1);
    lcd.print(buffer);
}

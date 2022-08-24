#include "custom-mode-entry-label.hpp"
#include "../../../devices/lcd/lcd.hpp"
#include "../../../strings/formats/number-format-strings.hpp"
#include "../../../strings/formats/date-time-format-strings.hpp"

CustomModeEntryLabel::CustomModeEntryLabel(CustomModeEntryLabelArgs &args)
    : args(args) {

}

void CustomModeEntryLabel::initialize() {    
    restoreLcd();
}

void CustomModeEntryLabel::display() {
    static const int OUTPUT_BUFER_LENGHT = 12;
    char buffer[OUTPUT_BUFER_LENGHT] = { '\0' };

    int displayIndex = this->args.index + 1;
    snprintf_P(
        buffer,
        OUTPUT_BUFER_LENGHT,
        (const char *)(&twoNumberPrefixFormat[0]),
        displayIndex);

    lcd.print(buffer);

    int padding = strnlen(buffer, OUTPUT_BUFER_LENGHT);

    TimeSpan startAt(this->args.startAt);
    TimeSpan duration(this->args.duration);
    snprintf_P(buffer, OUTPUT_BUFER_LENGHT, (const char *)(&timeSpanFormat[0]),
        startAt.days(),
        startAt.hours(),
        startAt.minutes(),
        startAt.seconds());
    lcd.print(buffer);

    lcd.setCursor(padding, 1);
    snprintf_P(buffer, OUTPUT_BUFER_LENGHT, (const char *)(&timeSpanFormat[0]), 
        duration.days(),
        duration.hours(),
        duration.minutes(),
        duration.seconds());
    lcd.print(buffer);
}

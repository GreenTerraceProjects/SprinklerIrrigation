#include "water-hammer-delay-label.hpp"
#include "../../../../devices/lcd/lcd.hpp"

WaterHammerDelayLabel::WaterHammerDelayLabel(WaterHammerDelayLabelArgs &args)
    : Label(args) {

}

void WaterHammerDelayLabel::initialize() {
    restoreLcd();
}

void WaterHammerDelayLabel::display() {
    const int OUTPUT_BUFFER_SIZE = LCD_COLS + 1;
    char buffer[OUTPUT_BUFFER_SIZE] = { '\0' };

    snprintf_P(
        buffer,
        OUTPUT_BUFFER_SIZE,
        PSTR("[0; %u] ms"),
        this->args.maxValue);
    lcd.print(buffer);
    lcd.setCursor(0, 1);
    lcd.print(this->args.value);
}

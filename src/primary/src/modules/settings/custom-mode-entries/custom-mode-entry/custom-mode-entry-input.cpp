#include <RTClib.h>
#include "custom-mode-entry-input.hpp"
#include "../../../inputs/timespan-input/timespan-input.hpp"
#include "custom-mode-entry-label.hpp"
#include "../../../devices/lcd/lcd.hpp"
#include "../../../devices/buzzer/buzzer.hpp"

CustomModeEntryInput::CustomModeEntryInput(CustomModeEntryInputArgs &args)
    :
    lineIndex(0),
    startAt(args.defaultStartAt),
    duration(args.defaultDuration),
    Input(args) {

}

void CustomModeEntryInput::initialize() {
    this->lineIndex = 0;
}

bool CustomModeEntryInput::handleInput(char inputKey) {
    switch (inputKey) {
        case '2':
            --this->lineIndex;
            if (this->lineIndex < 0) {
                this->lineIndex = 1;
            }
            break;
        case '8':
            ++this->lineIndex;
            if (this->lineIndex > 1) {
                this->lineIndex = 0;
            }
            break;
        case '5':
            this->inputTimeSpan();
            break;
        case 'A':
            return false;
        case 'C':
            this->cancel();
            return false;
        default:
            break;
    };

    this->viewEntry();
    return true;
}

void CustomModeEntryInput::viewEntry() {
    CustomModeEntryLabelArgs customModeEntryLabelArgs {
        .index = this->args.index,
        .startAt = this->startAt,
        .duration = this->duration
    };
    CustomModeEntryLabel customModeEntryLabel(customModeEntryLabelArgs);
    customModeEntryLabel.view();

    lcd.setCursor(LCD_COLS - 1, this->lineIndex);
    lcd.blink();
};

void CustomModeEntryInput::inputTimeSpan() {
    TimeSpan defaultTimeSpan = this->lineIndex == 0 ?
                                    this->startAt
                                        : this->duration;
    bool isFullTimeSpan      = this->lineIndex == 0 ? false : true;
    const uint32_t
        MAX_TIMESPAN_IN_SEC  = this->lineIndex == 0 ? 86400u - 1u : 86400u;

    TimeSpanInputArgs timeSpanInputArgs {
        .isFullTimeSpan = isFullTimeSpan,
        .defaultValue = defaultTimeSpan
    };
    TimeSpanInput timeSpanInput(timeSpanInputArgs);
    
    while (true) {
        timeSpanInput.run();
        if (timeSpanInput.isCanceled()) {
            return;
        }

        TimeSpanInputResult timeSpanInputResult = timeSpanInput.getResult();
        if (timeSpanInputResult.value.totalseconds() > MAX_TIMESPAN_IN_SEC) {
            lowBeep();

            timeSpanInputArgs.defaultValue = timeSpanInputResult.value;
            continue;
        }


        uint32_t *assignDest = this->lineIndex == 0 ? &this->startAt : &this->duration;
        *assignDest = timeSpanInputResult.value.totalseconds();
        break;
    }
}

CustomModeEntryInputResult CustomModeEntryInput::createResult() {
    return CustomModeEntryInputResult {
        .startAt = this->startAt,
        .duration = this->duration
    };
}

#include "cyclic-mode-setting-input.hpp"
#include "../../../inputs/timespan-input/timespan-input.hpp"
#include "../../../devices/buzzer/buzzer.hpp"

CylicModeSettingInput::CylicModeSettingInput(CylicModeSettingInputArgs &args)
    : value(0), Input(args) {

}

void CylicModeSettingInput::initialize() {
    this->value = 0;
    this->doInput();
}

bool CylicModeSettingInput::handleInput(char inputKey) {
    return false;
}

CylicModeSettingInputResult CylicModeSettingInput::createResult() {
    return CylicModeSettingInputResult {
        .value = this->value
    };
}

void CylicModeSettingInput::doInput() {
    TimeSpanInputArgs args {
        .isFullTimeSpan = this->args.isFullTimeSpan,
        .defaultValue = this->args.defaultValue
    };
    TimeSpanInput input(args);
    while (true) {
        input.run();
        if (input.isCanceled()) {
            this->cancel();
            return;
        }

        TimeSpanInputResult result = input.getResult();
        if (result.value.totalseconds() > this->args.maxValue) {
            lowBeep();
            args.defaultValue = result.value;
            continue;
        }

        this->value = result.value.totalseconds();        
        break;
    }
}

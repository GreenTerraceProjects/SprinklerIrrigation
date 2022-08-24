#include "cylic-mode-setting.hpp"
#include "../../../labels/time-span-label/time-span-label.hpp"
#include "../../../inputs/timespan-input/timespan-input.hpp"
#include "../../../devices/keypad/keypad.hpp"
#include "../../../devices/buzzer/buzzer.hpp"
#include <EEPROM.h>

CyclicModeSetting::CyclicModeSetting(CyclicModeSettingArgs &args)
    : args(args), Input(&keypad) {

}

void CyclicModeSetting::initialize() {

}

bool CyclicModeSetting::handleKeyPressed(char inputKey) {
    switch (inputKey) {
        case '5':
            this->inputSetting();
            break;
        case '*':
            this->cancel();
            return false;
        default:
            break;
    }

    this->showSetting();
    return true;
}

uint32_t CyclicModeSetting::readSetting() {
    uint32_t value;
    EEPROM.get(this->args.address, value);
    return value;
}

void CyclicModeSetting::writeSetting(uint32_t value) {
    EEPROM.put(this->args.address, value);
}

void CyclicModeSetting::showSetting() {
    TimeSpanLabelArgs args {
        .value = this->readSetting(),
        .isFullTimeSpan = this->args.isFullTimeSpan,
        .label = this->args.title
    };

    TimeSpanLabel label(args);
    label.view();
}

void CyclicModeSetting::inputSetting() {
    TimeSpanInputArgs args {
        .isFullTimeSpan = this->args.isFullTimeSpan,
        .defaultValue = this->readSetting()
    };
    TimeSpanInput input(args);
    while (true) {
        input.run();
        if (input.isCanceled()) {
            this->cancel();
            return;
        }

        TimeSpan timeSpan = input.getTimeSpan();
        if (timeSpan.totalseconds() > this->args.maxValue) {
            lowBeep();
            args.defaultValue = timeSpan;
            continue;
        }

        uint32_t writtenValue = timeSpan.totalseconds();
        this->writeSetting(writtenValue);

        break;
    }
}

#include "cylic-mode-setting.hpp"
#include "cyclic-mode-setting-label.hpp"
#include "cyclic-mode-setting-input.hpp"
#include "cyclic-mode-setting-reader.hpp"
#include "cyclic-mode-setting-writer.hpp"

CyclicModeSetting::CyclicModeSetting(CyclicModeSettingArgs &args)
    : Input(args) {

}

void CyclicModeSetting::initialize() {

}

bool CyclicModeSetting::handleInput(char inputKey) {
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

CyclicModeSettingResult CyclicModeSetting::createResult() {
    return CyclicModeSettingResult();
}

uint32_t CyclicModeSetting::readSetting() {
    CyclicModeSettingReaderArgs args {
        .address =  this->args.address
    };
    CyclicModeSettingReader reader(args);
    reader.run();
    CyclicModeSettingReaderResult result = reader.getResult();
    return result.value;
}

void CyclicModeSetting::writeSetting(uint32_t value) {
    CyclicModeSettingWriterArgs args {
        .address = this->args.address,
        .value = value
    };
    CyclicModeSettingWriter writer(args);
    writer.run();
}

void CyclicModeSetting::showSetting() {
    CyclicModeSettingLabelArgs args = {
        .value = this->readSetting(),
        .isFullTimeSpan = this->args.isFullTimeSpan,
        .title = this->args.title
    };
}

void CyclicModeSetting::inputSetting() {
    CylicModeSettingInputArgs args {
        .defaultValue = this->readSetting(),
        .isFullTimeSpan = this->args.isFullTimeSpan,
        .maxValue = this->args.maxValue
    };
    CylicModeSettingInput input(args);
    input.run();

    if (!input.isCanceled()) {
        CylicModeSettingInputResult result = input.getResult();
        this->writeSetting(result.value);
    }
}

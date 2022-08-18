#include "cyclic-mode-setting-label.hpp"
#include "../../../labels/time-span-label/time-span-label.hpp"

CyclicModeSettingLabel::CyclicModeSettingLabel(CyclicModeSettingLabelArgs &args)
    : Label(args) {

}

void CyclicModeSettingLabel::initialize() {

}

void CyclicModeSettingLabel::display() {
    TimeSpanLabelArgs args {
        .value = this->args.value,
        .isFullTimeSpan = this->args.isFullTimeSpan,
        .label = this->args.title
    };

    TimeSpanLabel label(args);
    label.view();
}

#include "eeprom-cleaning.hpp"
#include "../../inputs/dialog/dialog.hpp"
#include <avr/pgmspace.h>
#include <EEPROM.h>

const char warningMessage[] PROGMEM = "Do you want to continue?";
const char finishMessage[]  PROGMEM = "Success!";

EEPROMCleaning::EEPROMCleaning()
    : Input() {

}

void EEPROMCleaning::initialize() {
    if (!this->confirmCleaning()) {
        return;
    }

    this->clean();
    this->informCleaned();
}

bool EEPROMCleaning::handleKeyPressed(char inputKey) {
    return false;
}

bool EEPROMCleaning::confirmCleaning() {
    DialogArgs confirmDialogArgs {
        .type = DialogArgs::DialogType::YESNO,
        .message = warningMessage
    };

    Dialog confirmDialog(confirmDialogArgs);
    confirmDialog.run();

    DialogResult confirmDialogResult = confirmDialog.getResult();

    return (confirmDialogResult.result == DialogResult::Result::YES);
}

void EEPROMCleaning::informCleaned() {
    DialogArgs informDialogArgs {
        .type = DialogArgs::DialogType::OK,
        .message = finishMessage
    };
    Dialog informDialog(informDialogArgs);
    informDialog.run();
}

void EEPROMCleaning::clean() {
    for (int index = 0; index < EEPROM.length(); ++index) {
        EEPROM.write(index, 0);
    }
}

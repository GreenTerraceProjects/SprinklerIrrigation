#include "eeprom-cleaning.hpp"
#include "eeprom-cleaning-writer.hpp"
#include "../../inputs/dialog/dialog.hpp"
#include <avr/pgmspace.h>

const char warningMessage[] PROGMEM = "Do you want to continue?";
const char finishMessage[]  PROGMEM = "Success!";

EEPROMCleaning::EEPROMCleaning(EEPROMCleaningArgs &args)
    : Input(args) {

}

void EEPROMCleaning::initialize() {
    DialogArgs confirmDialogArgs {
        .type = DialogArgs::DialogType::YESNO,
        .message = warningMessage
    };

    Dialog confirmDialog(confirmDialogArgs);
    confirmDialog.run();

    DialogResult confirmDialogResult = confirmDialog.getResult();
    if (confirmDialogResult.result == DialogResult::Result::YES) {
        EEPROMCleaningWriterArgs args;
        EEPROMCleaningWriter resetEEPROMWriter(args);
        resetEEPROMWriter.run();

        DialogArgs informDialogArgs {
            .type = DialogArgs::DialogType::OK,
            .message = finishMessage
        };
        Dialog informDialog(informDialogArgs);
        informDialog.run();
    }
}

bool EEPROMCleaning::handleInput(char inputKey) {
    return false;
}

EEPROMCleaningResult EEPROMCleaning::createResult() {
    return EEPROMCleaningResult();
}

#include "dialog.hpp"
#include "../input.hpp"
#include "../../devices/lcd/lcd.hpp"

Dialog::Dialog(DialogArgs &args) : Input(args) {
    
}

void Dialog::initialize() {
    this->wordWrapLine = 1;
    this->wordWrapMovingDirection = WordWrapMovingDirection::NotChange;
    restoreLcd();
    this->refreshDisplay();
}

bool Dialog::handleInput(char inputKey) {
    switch (inputKey) {
        case '4':
            this->wordWrapMovingDirection = WordWrapMovingDirection::Backward;
            break;
        case '6':
            this->wordWrapMovingDirection = WordWrapMovingDirection::Forward;
            break;
        case 'A':
            switch (this->args.type) {
                case DialogArgs::DialogType::OK:
                    this->selectedResult = DialogResult::Result::OK;
                    break;
                case DialogArgs::DialogType::YESNO:
                    this->selectedResult = DialogResult::Result::YES;
                    break;
                default:
                    break;
            }
            return false;
            break;
        case 'C':
            this->cancel();
            return false;
        default:
            break;
    }

    this->refreshDisplay();
    return true;
}

DialogResult Dialog::createResult() {
    DialogResult dialogResult;
    
    if (this->isCanceled()) {
        switch (this->args.type) {
            case DialogArgs::DialogType::OK:
                dialogResult.result = DialogResult::Result::OK;
                break;
            case DialogArgs::DialogType::YESNO:
                dialogResult.result = DialogResult::Result::NO;
                break;
            default:
                break;
        }
    } else {
        dialogResult.result = this->selectedResult;
    }

    return dialogResult;
}

void Dialog::refreshDisplay() {
    const int bufferSize = LCD_COLS + 1;
    char buffer[bufferSize] = { '\0' };

    WordWrap wordWrap(this->args.message, buffer, bufferSize);
    if (this->wordWrapMovingDirection == WordWrapMovingDirection::Forward) {
        if (wordWrap.goTo(this->wordWrapLine + 1)) {
            this->wordWrapLine += 1;
        }
    } else if (this->wordWrapMovingDirection == WordWrapMovingDirection::Forward) {
        if (wordWrap.goTo(this->wordWrapLine - 1)) {
            this->wordWrapLine -= 1;
        }
    } else {
        wordWrap.goTo(this->wordWrapLine);
    }

    if (this->wordWrapMovingDirection != WordWrapMovingDirection::NotChange) {
        this->wordWrapMovingDirection = WordWrapMovingDirection::NotChange;
    }

    lcd.setCursor(0, 0);
    lcd.print(buffer);

    const char *actionsStr = this->getActionString();
    const int actionsStrLen = strlen_P(actionsStr);
    // AVR LIBC does not support "%*s" format in sprintf/snprintf.
    lcd.setCursor(0, 1);
    for (int i = 0; i < (LCD_COLS - actionsStrLen) / 2; ++i) {
      lcd.print(' ');
    }
    lcd.print((const __FlashStringHelper *)actionsStr);
    for (int i = 0; i < (LCD_COLS - actionsStrLen) / 2; ++i) {
      lcd.print(' ');
    }
}

const char *Dialog::getActionString() {
    static const char selection_yesno_str[]   PROGMEM = "(C)No (A)Yes";
    static const char selection_ok_str[]      PROGMEM = "(A)OK";
    static const char selection_unknown_str[] PROGMEM = "(A)!@#%*";

    switch (this->args.type) {
        case DialogArgs::DialogType::OK:
            return selection_ok_str;
        case DialogArgs::DialogType::YESNO:
            return selection_yesno_str;
        default:
            return selection_unknown_str;
    }
}

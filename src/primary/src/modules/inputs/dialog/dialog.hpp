#ifndef _DIALOG_HPP_
#define _DIALOG_HPP_

#include "../input.hpp"
#include "../../strings/wordwrap/wordwrap.hpp"

class DialogArgs {
public:
    enum DialogType {
        YESNO,
        OK
    };

public:
    enum DialogType type;
    const char *message;    
};

class DialogResult {
public:
    enum Result {
        UNKNOWN,
        YES,
        NO,
        OK
    };

public:
    enum Result result;
};

class Dialog: public Input {

public:
    Dialog(DialogArgs &args);

private:
    DialogArgs &args;

private:
    int wordWrapLine;
    enum DialogResult::Result selectedResult;
    enum WordWrapMovingDirection wordWrapMovingDirection;

private:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

public:
    DialogResult getResult();

private:
    const char *getActionString();
    void refreshDisplay();
};

#endif

#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include <Keypad.h>

class Input {
protected:
    Input();
    Input(Keypad *keyPad);

protected:
    Keypad *keyPad;
    bool canceled;

public:
    bool isCanceled();
    void cancel();

public:
    virtual void run();

protected:
    virtual void initialize() = 0;
    virtual bool handleKeyPressed(char key) = 0;
    virtual bool poll();
    virtual bool handlePolling();
};

#endif

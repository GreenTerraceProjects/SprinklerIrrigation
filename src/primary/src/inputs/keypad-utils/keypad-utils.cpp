#include "keypad-utils.hpp"
#include "../../devices/keypad/keypad.hpp"
#include "../too-long-to-loop/too-long-to-loop.hpp"

bool waitForKeyPressed(char &key) {
    while (!tooLongToLoop()) {
        key = keypad.getKey();
        if (!key) continue;
        return true;
    }

    return false;
}

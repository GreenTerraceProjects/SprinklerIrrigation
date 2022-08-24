#include "input.hpp"
#include "../devices/watch-dog/watch-dog.hpp"
#include "../devices/keypad/keypad.hpp"

Input::Input() : Input(&keypad) {

}

Input::Input(Keypad *keyPad): canceled(false), keyPad(keyPad) {

}

bool Input::isCanceled() {
    return this->canceled;
}

void Input::cancel() {
    this->canceled = true;
}

void Input::initialize() {
    this->canceled = false;
}

bool Input::poll() {
    return false;
}

bool Input::handlePolling() {
    return true;
}

void Input::run() {
    this->Input::initialize();
    this->initialize();
    
    while (true) {
        if (this->isCanceled()) {
            break;
        }

        if (tooLongToLoop()) {
            this-> cancel();
            break;
        }

        if (this->poll()) {
            if (!this->handlePolling()) {
                break;
            }
        }

        char key = this->keyPad->getKey();
        if (!key) continue;
        if (!this->handleKeyPressed(key)) {
            break;
        }
    }
}

#ifndef _INPUT_WITH_POLLING_HPP_
#define _INPUT_WITH_POLLING_HPP_

#include "input.hpp"

template <typename TResult, typename TArgs>
class InputWithPolling : public Input <TResult, TArgs> {
protected:
    InputWithPolling(TArgs &args);

public:
    virtual void run() override;

protected:
    virtual bool poll();
    virtual bool handlePolling();
};

template <typename TResult, typename TArgs>
InputWithPolling<TResult, TArgs>::InputWithPolling(TArgs &args)
    : Input<TResult, TArgs>(args) {

}

template<typename TResult, typename TArgs>
void InputWithPolling<TResult, TArgs>::run() {
    InputWithPolling::initialize();
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

        char pressedKey = keypad.getKey();
        if (!pressedKey) continue;

        if (!this->handleInput(pressedKey)) {
            break;
        }
    }

    this->result = this->createResult();
}

template<typename TResult, typename TArgs>
bool InputWithPolling<TResult, TArgs>::poll() {
    return false;
}

template<typename TResult, typename TArgs>
bool InputWithPolling<TResult, TArgs>::handlePolling() {
    return true;
}

#endif

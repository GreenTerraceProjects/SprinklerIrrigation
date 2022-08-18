#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include "../devices/keypad/keypad.hpp"
#include "watch-dog/watch-dog.hpp"

template<typename TResult, typename TArgs>
class Input {
protected:
    TArgs& args;
    TResult result;

private:
    bool canceled;

protected:
    Input(TArgs &args);

public:
    virtual void run();
    void cancel();

protected:
    virtual void initialize();
    virtual bool handleInput(char inputKey);
    virtual TResult createResult() = 0;

public:
    bool isCanceled();
    TResult getResult();
};

template<typename TResult, typename TArgs>
Input<TResult, TArgs>::Input(TArgs &args)
    : args(args), canceled(false) {

}

template<typename TResult, typename TArgs>
void Input<TResult, TArgs>::run() {
    Input::initialize();
    this->initialize();
    
    while (true) {
        if (this->isCanceled()) {
            break;
        }

        if (tooLongToLoop()) {
            this-> cancel();
            break;
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
void Input<TResult, TArgs>::initialize() {
    this->canceled = false;
}

template<typename TResult, typename TArgs>
bool Input<TResult, TArgs>::handleInput(char inputKey) {
    return true;
}

template<typename TResult, typename TArgs>
bool Input<TResult, TArgs>::isCanceled() {
    return this->canceled;
}

template<typename TResult, typename TArgs>
void Input<TResult, TArgs>::cancel() {
    this->canceled = true;
}

template<typename TResult, typename TArgs>
TResult Input<TResult, TArgs>::getResult() {
    return this->result;
}

#endif

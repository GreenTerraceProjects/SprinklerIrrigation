#include "water-hammer-delay-input.hpp"
#include "../../../../inputs/unsigned-integer-input/unsigned-integer-input.hpp"
#include "../../../../devices/buzzer/buzzer.hpp"

WaterHammerDelayInput::WaterHammerDelayInput(WaterHammerDelayInputArgs &args)
    : value(args.defaultValue), Input(args) {

}

void WaterHammerDelayInput::initialize() {
    this->value = this->args.defaultValue;
    this->doInputUnsignedInteger();
}

bool WaterHammerDelayInput::handleInput(char inputKey) {
    return false;
}

WaterHammerDelayInputResult WaterHammerDelayInput::createResult() {
    return WaterHammerDelayInputResult {
        .value = this->value
    };
}

void WaterHammerDelayInput::doInputUnsignedInteger() {
    UnsignedIntegerInputArgs unsignedIntegerInputArgs {
        .defaultValue = this->args.defaultValue
    };

    UnsignedIntegerInput unsignedIntegerInput(unsignedIntegerInputArgs);
    while (true) {
        unsignedIntegerInput.run();

        if (unsignedIntegerInput.isCanceled()) {
            this->cancel();
            return;
        }

        UnsignedIntegerInputResult unsignedIntegerInputResult = unsignedIntegerInput.getResult();
        if (unsignedIntegerInputResult.value > this->args.maxValue) {
            unsignedIntegerInputArgs.defaultValue = unsignedIntegerInputResult.value;
            lowBeep();
            continue;
        }

        this->value = unsignedIntegerInputResult.value;
        break;
    }
}

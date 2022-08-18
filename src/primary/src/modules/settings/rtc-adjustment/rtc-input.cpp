#include "rtc-input.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../inputs/date-time-input/date-time-input.hpp"

RtcInput::RtcInput(RtcInputArgs &args)
  : Input(args) {

}

void RtcInput::initialize() {
  
}

bool RtcInput::handleInput(char inputKey) {
  return false;
}

RtcInputResult RtcInput::createResult() {
  DateTimeInputArgs dateTimeInputArgs {
    .defaultValue = this->args.defaultValue
  };
  DateTimeInput dateTimeInput(dateTimeInputArgs);
  dateTimeInput.run();

  if (dateTimeInput.isCanceled()) {
    this->cancel();
    return RtcInputResult {
      .value = this->args.defaultValue
    };
  }

  DateTimeInputResult dateTimeInputResult = dateTimeInput.getResult();
  return RtcInputResult {
    .value = dateTimeInputResult.value
  };
}

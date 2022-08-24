#include "timespan-input.hpp"
#include "../date-time-utils/date-time-utils.hpp"
#include "../../strings/formats/date-time-format-strings.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/buzzer/buzzer.hpp"
#include "../../devices/keypad/keypad.hpp"

TimeSpanInput::TimeSpanInput(TimeSpanInputArgs &args) : args(args), Input(&keypad) {

}

void TimeSpanInput::initialize() {
  restoreLcd();
  lcd.cursor();

  this->setupDisplayFrame();
  this->refreshDisplay();
}

bool TimeSpanInput::handleKeyPressed(char inputKey) {
  switch (inputKey) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      this->inputBuffer[this->index] = inputKey;
      if (this->index < strlen(this->inputBuffer) - 1) {
        ++this->index;

        if (isDateTimeDelimiter(this->inputBuffer[this->index])) {
          ++this->index;
        }
      }
      break;
    case '*':
      --this->index;
      if (isDateTimeDelimiter(this->inputBuffer[this->index])) {
        --this->index;
      }
      if (this->index < 0) {
        this->index = strlen(this->inputBuffer) - 1;
      }
      break;
    case '#':
      ++this->index;
      if (isDateTimeDelimiter(this->inputBuffer[this->index])) {
        ++this->index;
      }
      if (this->index >= strlen(this->inputBuffer)) {
        this->index = 0;
      }
      break;
    case 'A':
      if (this->args.isFullTimeSpan) {
        sscanf_P(this->inputBuffer, (const char *)(&timeSpanScanFormat[0]),
        &this->days, &this->hours, &this->minutes, &this->seconds);
      } else {
        sscanf_P(this->inputBuffer, (const char *)(&timeScanFormat[0]),
        &this->hours, &this->minutes, &seconds);
        this->days = 0;
      }
      if (this->days < 0 || this->days > 99 || this->hours < 0 || this->hours > 23 || this->minutes < 0 || this->minutes > 59 || this->seconds < 0 || this->seconds > 59) {
        lowBeep();
      } else {
        return false;
      }
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

TimeSpan TimeSpanInput::getTimeSpan() {  
  if (this->isCanceled()) {
    return this->args.defaultValue;
  }

  return TimeSpan(
    this->days,
    this->hours,
    this->minutes,
    this->seconds);
}

void TimeSpanInput::setupDisplayFrame() {
  if (this->args.isFullTimeSpan) {
    lcd.print(F("Set DD-HH:MM:SS"));
  } else {
    lcd.print(F("Set HH:MM:SS"));
  }

  if (this->args.isFullTimeSpan) {
    snprintf_P(this->inputBuffer, INPUT_BUFFER_LENGTH, (const char *)(&timeSpanFormat[0]),
    this->args.defaultValue.days(),
    this->args.defaultValue.hours(),
    this->args.defaultValue.minutes(),
    this->args.defaultValue.seconds());
  } else {
    // Reuse time format, because it is the same as short time span format.
    snprintf_P(this->inputBuffer, INPUT_BUFFER_LENGTH, (const char *)(&timeFormat[0]),
    this->args.defaultValue.hours(),
    this->args.defaultValue.minutes(),
    this->args.defaultValue.seconds());
  }
}

void TimeSpanInput::refreshDisplay() {
  lcd.setCursor(CUSOR_PADDING, 1);
  lcd.print(this->inputBuffer);
  lcd.setCursor(CUSOR_PADDING + this->index, 1);
}

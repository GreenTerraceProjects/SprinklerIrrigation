#include "date-time-input.hpp"
#include "../date-time-utils/date-time-utils.hpp"
#include "../../devices/lcd/lcd.hpp"
#include "../../devices/buzzer/buzzer.hpp"
#include "../../strings/labels/date-time-labels.hpp"
#include "../../strings/formats/date-time-format-strings.hpp"

DateTimeInput::DateTimeInput(DateTimeInputArgs &args) : Input(args) {

}

void DateTimeInput::initialize() {
  restoreLcd();
  lcd.cursor();

  this->lineIndex = 0;
  this->index0 = this->index1 = 0;

  this->setupDisplayFrame();
  this->refreshDisplay();
}

bool DateTimeInput::handleInput(char inputKey) {
  char *const inputBuffer = this->lineIndex == 0 ?
                            this->inputBuffer0 : this->inputBuffer1;
  int *const index = this->lineIndex == 0 ?
                     &this->index0 : &this->index1;

  bool edgeOfLine = false;

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
      inputBuffer[*index] = inputKey;
      if (*index < strlen(inputBuffer) - 1) {
        ++*index;

        if (isDateTimeDelimiter(inputBuffer[*index])) {
          ++*index;
        }
      } else {
        edgeOfLine = true;
      }

      if (lineIndex == 0 && edgeOfLine) {
        this->lineIndex = 1;
        this->index1 = 0;
      }
      break;
    case '*':
      --*index;
      if (isDateTimeDelimiter(inputBuffer[*index])) {
        --*index;
      }
      if (*index < 0) {
        edgeOfLine = true;
      }

      if (edgeOfLine) {
        if (this->lineIndex == 0) {
          this->lineIndex = 1;
          this->index1 = strlen(this->inputBuffer1) - 1;
        } else {
          this->lineIndex = 0;
          this->index0 = strlen(this->inputBuffer0) - 1;
        }
      }
      break;
    case '#':
        ++*index;
        if (isDateTimeDelimiter(inputBuffer[*index])) {
          ++*index;
        }
        if (*index >= strlen(inputBuffer)) {
          edgeOfLine = true;
        }

        if (edgeOfLine) {
          if (this->lineIndex == 0) {
            this->lineIndex = 1;
            this->index1 = 0;
          } else {
            this->lineIndex = 0;
            this->index0 = 0;
          }
        }
        break;
    case 'A':
      sscanf_P(this->inputBuffer0, (const char *)(&dateScanFormat[0]),
        &this->year, &this->month, &this->day);
      
      sscanf_P(this->inputBuffer1, (const char *)(&timeScanFormat[0]),
        &this->hour, &this->minute, &this->second);

      if (!DateTime(this->year, this->month, this->day,
                    this->hour, this->minute, this->second)
                    .isValid()) {
        
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

void DateTimeInput::setupDisplayFrame() {
  lcd.setCursor(0, 0);
  lcd.print((const __FlashStringHelper *)(&dateLabel[0]));
  lcd.setCursor(0, 1);
  lcd.print((const __FlashStringHelper *)(&timeLabel[0]));

  snprintf_P(this->inputBuffer1, INPUT_BUFFER_LENGTH, (const char *)(&dateFormat[0]), 
    this->args.defaultValue.year(),
    this->args.defaultValue.month(),
    this->args.defaultValue.day());
  
  snprintf_P(this->inputBuffer1, INPUT_BUFFER_LENGTH, (const char *)(&timeFormat[0]),
    this->args.defaultValue.hour(),
    this->args.defaultValue.minute(),
    this->args.defaultValue.second());
}

void DateTimeInput::refreshDisplay() {
  lcd.setCursor(CUSOR_PADDING, 0);
  lcd.print(this->inputBuffer0);
  lcd.setCursor(CUSOR_PADDING, 1);
  lcd.print(this->inputBuffer1);

  if (this->lineIndex == 0) {
    lcd.setCursor(CUSOR_PADDING + this->index0, 0);
  } else {
    lcd.setCursor(CUSOR_PADDING + this->index1, 1);
  }
}

DateTimeInputResult DateTimeInput::createResult() {
  DateTimeInputResult result;
  
  if (this->isCanceled()) {
    result.value = this->args.defaultValue;
  } else {
    result.value = DateTime(
      this->year,
      this->month,
      this->day,
      this->hour,
      this->minute,
      this->second);
  }

  return result;
}

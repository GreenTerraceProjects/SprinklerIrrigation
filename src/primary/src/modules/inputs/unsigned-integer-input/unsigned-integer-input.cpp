#include "unsigned-integer-input.hpp"
#include "../../devices/lcd/lcd.hpp"

UnsignedIntegerInput::UnsignedIntegerInput(UnsignedIntegerInputArgs &args) : args(args), Input() {

}

void UnsignedIntegerInput::initialize() {
  this->index = 0;
  memset(this->inputBuffer, '\0', INPUT_BUFFER_LENGTH);
  if (this->args.defaultValue != 0) {
    snprintf_P(this->inputBuffer, INPUT_BUFFER_LENGTH, PSTR("%lu"), this->args.defaultValue);
  }

  restoreLcd();
  lcd.print(F("Enter value:"));
  lcd.cursor();
}

bool UnsignedIntegerInput::handleKeyPressed(char inputKey) {
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
      memmove(
        &this->inputBuffer[this->index + 1],
        &this->inputBuffer[this->index],
        strlen(&this->inputBuffer[this->index]));
      this->inputBuffer[NUMBER_LENGTH] = '\0';
      this->inputBuffer[this->index] = inputKey;
      
      if (this->index < NUMBER_LENGTH - 1) {
        ++this->index;
      }
      break;
    case '*':
      --this->index;
      if (this->index < 0) {
        this->index = strlen(this->inputBuffer);
        if (this->index >= NUMBER_LENGTH) {
          this->index = NUMBER_LENGTH - 1;
        }
      }
      break;
    case '#':
      ++this->index;
      if (this->index > strlen(this->inputBuffer) || this->index >= NUMBER_LENGTH) {
        this->index = 0;
      }
      break;
    case 'B':
      if (index > 0) {
        memmove(
          &this->inputBuffer[this->index - 1],
          &this->inputBuffer[this->index],
          strlen(&this->inputBuffer[this->index]) + 1
        );
        --this->index;
      }
      break;
    case 'D':
      if (this->index < strlen(this->inputBuffer)) {
        memmove(
          &this->inputBuffer[this->index],
          &this->inputBuffer[this->index + 1],
          strlen(&this->inputBuffer[this->index + 1]) + 1
        );
      }
      break;
    case 'A':
      return false;
    case 'C':
      this->cancel();
      return false;
    default:
      break;
  }

  memset(
    &this->inputBuffer[strlen(this->inputBuffer)],
    '\0',
    INPUT_BUFFER_LENGTH - strlen(this->inputBuffer)
  );
  this->refreshDisplay();
  return true;
}

void UnsignedIntegerInput::refreshDisplay() {
  clearLcdRow(1);
  lcd.setCursor(0, 1);
  lcd.print(this->inputBuffer);
  lcd.setCursor(this->index, 1);
}

uint32_t UnsignedIntegerInput::getValue() {
  if (this->isCanceled()) {
    return this->args.defaultValue;
  }

  return strtoul(this->inputBuffer, NULL, 10);
}

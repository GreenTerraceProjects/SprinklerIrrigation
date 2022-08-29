#include "datetime-utils.hpp"

bool isDateTimeDelimiter(char character) {
  return character == '-' || character == ':';
}

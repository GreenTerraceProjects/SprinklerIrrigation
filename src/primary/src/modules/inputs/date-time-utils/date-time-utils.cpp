#include "date-time-utils.hpp"

bool isDateTimeDelimiter(char character) {
  return character == '-' || character == ':';
}

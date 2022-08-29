#include "datetime-formats.hpp"
#include <avr/pgmspace.h>

// "99-23:59:59"
const char timeSpanFormat[] PROGMEM = "%02d-%02d:%02d:%02d";
const char timeSpanScanFormat[] PROGMEM = "%d-%d:%d:%d";
// "2022-31-01"
const char dateFormat[] PROGMEM = "%04d-%02d-%02d";
const char dateScanFormat[] PROGMEM = "%d-%d-%d";
// "23:59:59"
const char timeFormat[] PROGMEM = "%02d:%02d:%02d";
const char timeScanFormat[] PROGMEM = "%d:%d:%d";

#include "eeprom-cleaning.hpp"
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "../../inputs/dialog/dialog.hpp"

const char warningMessage[] PROGMEM = "Do you want to continue?";
const char finishMessage[]  PROGMEM = "Success!";

void clearEEPROM() {
  if (displayDialog(warningMessage, YESNO)){
    for (int i = 0; i < EEPROM.length(); ++i) {
      EEPROM.write(i, 0);
    }

    displayDialog(finishMessage, OK);
  }
}

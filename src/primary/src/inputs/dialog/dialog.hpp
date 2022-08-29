#ifndef _DIALOG_HPP_
#define _DIALOG_HPP_

#include <stdint.h>

enum DialogType {
  YESNO,
  OK
};

int8_t displayDialog(const char* message, enum DialogType dialogType);

#endif

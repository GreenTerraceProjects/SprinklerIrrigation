#ifndef _TOO_LONG_TO_LOOP_HPP_
#define _TOO_LONG_TO_LOOP_HPP_

#include <RTClib.h>

extern DateTime lastInteract;

bool tooLongToLoop();
void resetLastInteractTimer();
void setupTooLongToLoop();

#endif

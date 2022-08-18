#ifndef _WATCH_DOG_HPP_
#define _WATCH_DOG_HPP_

#include <RTClib.h>

extern DateTime lastInteract;

bool tooLongToLoop();
void resetLastInteractTimer();

#endif
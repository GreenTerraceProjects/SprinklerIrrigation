#ifndef _PUMP_HPP_
#define _PUMP_HPP_

#include <RTClib.h>

extern bool isPumpOn;

extern DateTime pumpStartedAt;
extern DateTime pumpStoppedAt;

void startPump();
void stopPump();
void setupRelays();
void setupPumpTimers();

#endif

#ifndef _PUMP_HPP_
#define _PUMP_HPP_

#include <RTClib.h>

extern bool isPumpOn;
extern unsigned long pumpOperationChangedAt;
extern bool safeToRunPumpMotor;
extern bool safeToCloseValve;

extern DateTime pumpStartedAt;
extern DateTime pumpStoppedAt;

void startPump();
void stopPump();

#endif

#ifndef _IRRIGATION_HPP_
#define _IRRIGATION_HPP_

enum IrrigationMode {
  ALWAYS_OFF,
  ALWAYS_ON,
  CYCLIC,
  CUSTOM,
  REMOTE_CONTROL_MCU
};

extern enum IrrigationMode irrigationMode;

#endif

#ifndef _SYSTEM_STATUS_HPP_
#define _SYSTEM_STATUS_HPP_

enum DisplayStatusType {
  DeviceTitle,
  CurrentDateTime,
  CurrentIrrigationMode,
  CurrentPumpStatus
};

void setDisplayStatus(enum DisplayStatusType type);
void displayStatus();

#endif

#ifndef _CUSTOM_MODE_ENTRY_HPP_
#define _CUSTOM_MODE_ENTRY_HPP_

#include <stdint.h>

void changeCustomModeSetting(int sortedIndex, uint32_t &startAt, uint32_t &duration, bool &isCanceled);
void displayCustomSetting(int sortedIndex, const uint32_t &startAt, const uint32_t &duration);

#endif

#ifndef _CUSTOM_MODE_ENTRY_READER_HPP_
#define _CUSTOM_MODE_ENTRY_READER_HPP_

#include "../../../errors/errors.hpp"
#include <stdint.h>

class CustomModeEntryReaderArgs {
public:
    int index;
};

class CustomModeEntryReaderResult {
public:
    uint32_t startAt;
    uint32_t duration;
};

class CustomModeEntryReader {
public:
    CustomModeEntryReader(CustomModeEntryReaderArgs &args);

protected:
    CustomModeEntryReaderArgs &args;

public:
    CustomModeEntryReaderResult read();
};

#endif

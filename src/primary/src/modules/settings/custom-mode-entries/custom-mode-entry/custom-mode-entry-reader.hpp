#ifndef _CUSTOM_MODE_ENTRY_READER_HPP_
#define _CUSTOM_MODE_ENTRY_READER_HPP_

#include "../../../errors/errors.hpp"
#include "../../setting-reader.hpp"
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

class CustomModeEntryReader
    : public SettingReader<CustomModeEntryReaderResult, CustomModeEntryReaderArgs> {
public:
    CustomModeEntryReader(CustomModeEntryReaderArgs &args);

protected:
    uint32_t startAt;
    uint32_t duration;

protected:
    enum ERROR read() override;
    CustomModeEntryReaderResult createResult() override;
};

#endif

#ifndef _CUSTOM_MODE_ENTRY_WRITER_HPP_
#define _CUSTOM_MODE_ENTRY_WRITER_HPP_

#include "../../../errors/errors.hpp"
#include <stdint.h>

class CustomModeEntryWriterArgs {
public:
    int index;
    uint32_t startAt;
    uint32_t duration;
};

class CustomModeEntryWriter{
public:
    CustomModeEntryWriter(CustomModeEntryWriterArgs &args);

protected:
    CustomModeEntryWriterArgs &args;

public:
    void write();
};

#endif

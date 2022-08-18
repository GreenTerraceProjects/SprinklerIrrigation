#ifndef _CUSTOM_MODE_ENTRY_WRITER_HPP_
#define _CUSTOM_MODE_ENTRY_WRITER_HPP_

#include "../../setting-writer.hpp"
#include "../../../errors/errors.hpp"
#include <stdint.h>

class CustomModeEntryWriterArgs {
public:
    int index;
    uint32_t startAt;
    uint32_t duration;
};

class CustomModeEntryWriterResult {

};

class CustomModeEntryWriter : public SettingWriter<CustomModeEntryWriterResult, CustomModeEntryWriterArgs>{
public:
    CustomModeEntryWriter(CustomModeEntryWriterArgs &args);

protected:
    enum ERROR write() override;
    CustomModeEntryWriterResult createResult() override;
};

#endif

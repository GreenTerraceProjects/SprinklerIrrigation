#ifndef _CYCLIC_MODE_SETTING_WRITER_HPP_
#define _CYCLIC_MODE_SETTING_WRITER_HPP_

#include "../../setting-writer.hpp"
#include <stdint.h>

class CyclicModeSettingWriterArgs {
public:
    int address;
    uint32_t value;
};

class CyclicModeSettingWriterResult {

};

class CyclicModeSettingWriter : public SettingWriter<CyclicModeSettingWriterResult, CyclicModeSettingWriterArgs> {
public:
    CyclicModeSettingWriter(CyclicModeSettingWriterArgs &args);

protected:
    enum ERROR write() override;
    CyclicModeSettingWriterResult createResult() override;
};

#endif

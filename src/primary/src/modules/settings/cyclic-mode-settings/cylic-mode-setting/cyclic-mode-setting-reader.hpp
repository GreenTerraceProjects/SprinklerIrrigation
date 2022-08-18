#ifndef _CYCLIC_MODE_SETTING_READER_HPP_
#define _CYCLIC_MODE_SETTING_READER_HPP_

#include "../../setting-reader.hpp"
#include <stdint.h>

class CyclicModeSettingReaderArgs {
public:
    int address;
};

class CyclicModeSettingReaderResult {
public:
    uint32_t value;
};

class CyclicModeSettingReader : public SettingReader<CyclicModeSettingReaderResult, CyclicModeSettingReaderArgs> {
public:
    CyclicModeSettingReader(CyclicModeSettingReaderArgs &args);

protected:
    uint32_t value;

protected:
    enum ERROR read() override;
    CyclicModeSettingReaderResult createResult() override;
};

#endif

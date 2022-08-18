#ifndef _WATER_HAMMER_DELAY_READER_HPP_
#define _WATER_HAMMER_DELAY_READER_HPP_

#include "../../../setting-reader.hpp"
#include <stdint.h>

class WaterHammerDelayReaderArgs {
public:
    int address;

};

class WaterHammerDelayReaderResult {
public:
    uint16_t value;
};

class WaterHammerDelayReader : public SettingReader<WaterHammerDelayReaderResult, WaterHammerDelayReaderArgs> {
public:
    WaterHammerDelayReader(WaterHammerDelayReaderArgs &args);

protected:
    uint16_t value;

protected:
    enum ERROR read() override;
    WaterHammerDelayReaderResult createResult() override;
};

#endif

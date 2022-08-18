#ifndef _WATER_HAMMER_DELAY_WRITER_HPP_
#define _WATER_HAMMER_DELAY_WRITER_HPP_

#include "../../../setting-writer.hpp"
#include <stdint.h>

class WaterHammerDelayWriterArgs {
public:
    int address;
    uint16_t value;
};

class WaterHammerDelayWriterResult {

};

class WaterHammerDelayWriter : public SettingWriter<WaterHammerDelayWriterResult, WaterHammerDelayWriterArgs> {
public:
    WaterHammerDelayWriter(WaterHammerDelayWriterArgs &args);

protected:
    enum ERROR write() override;
    WaterHammerDelayWriterResult createResult() override;
};

#endif

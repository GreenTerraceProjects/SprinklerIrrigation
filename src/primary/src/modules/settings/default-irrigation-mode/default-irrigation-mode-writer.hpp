#ifndef _DEFAULT_IRRIGATION_MODE_WRITER_HPP_
#define _DEFAULT_IRRIGATION_MODE_WRITER_HPP_

#include "../setting-writer.hpp"
#include <stdint.h>

class DefaultIrrigationModeWriterArgs {
public:
    uint8_t value;
};

class DefaultIrrigationModeWriterResult {

};

class DefaultIrrigationModeWriter : public SettingWriter<DefaultIrrigationModeWriterResult, DefaultIrrigationModeWriterArgs> {
public:
    DefaultIrrigationModeWriter(DefaultIrrigationModeWriterArgs &args);

protected:
    enum ERROR write() override;
    DefaultIrrigationModeWriterResult createResult() override;
};

#endif

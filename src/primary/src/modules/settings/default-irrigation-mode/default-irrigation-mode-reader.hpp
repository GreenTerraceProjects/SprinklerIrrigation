#ifndef _DEFAULT_IRRIGATION_MODE_READER_HPP_
#define _DEFAULT_IRRIGATION_MODE_READER_HPP_

#include "../setting-reader.hpp"
#include <stdint.h>

class DefaultIrrigationModeReaderArgs {

};

class DefaultIrrigationModeReaderResult {
public:
    uint8_t value;
};

class DefaultIrrigationModeReader : public SettingReader<DefaultIrrigationModeReaderResult, DefaultIrrigationModeReaderArgs> {
public:
    DefaultIrrigationModeReader(DefaultIrrigationModeReaderArgs &args);

protected:
    uint8_t value;

protected:
    enum ERROR read() override;
    DefaultIrrigationModeReaderResult createResult() override;
};

#endif

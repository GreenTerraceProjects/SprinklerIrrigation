#include "water-hammer-delay-writer.hpp"
#include <EEPROM.h>

WaterHammerDelayWriter::WaterHammerDelayWriter(WaterHammerDelayWriterArgs &args)
    : SettingWriter(args) {

}

enum ERROR WaterHammerDelayWriter::write() {
    EEPROM.put(this->args.address, this->args.value);
    return ERROR::NONE;
}

WaterHammerDelayWriterResult WaterHammerDelayWriter::createResult() {
    return WaterHammerDelayWriterResult();
}

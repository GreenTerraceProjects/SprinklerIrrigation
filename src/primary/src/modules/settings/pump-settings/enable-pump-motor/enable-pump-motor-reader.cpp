#include "enable-pump-motor-reader.hpp"
#include "../../../devices/eeprom/addresses.hpp"
#include <EEPROM.h>

EnablePumpMotorReader::EnablePumpMotorReader(EnablePumpMotorReaderArgs &args)
    : value(false), SettingReader(args) {

}

enum ERROR EnablePumpMotorReader::read() {
    EEPROM.get(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, this->value);
    return ERROR::NONE;
}

EnablePumpMotorReaderResult EnablePumpMotorReader::createResult() {
    return EnablePumpMotorReaderResult {
        .value = this->value
    };
}

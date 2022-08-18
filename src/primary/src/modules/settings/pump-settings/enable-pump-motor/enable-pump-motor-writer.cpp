#include "enable-pump-motor-writer.hpp"
#include "../../../devices/eeprom/addresses.hpp"
#include <EEPROM.h>

EnablePumpMotorWriter::EnablePumpMotorWriter(EnablePumpMotorWriterArgs &args)
    : SettingWriter(args) {

}

enum ERROR EnablePumpMotorWriter::write() {
    EEPROM.put(EEPROM_PUMP_MOTOR_ENABLED_ADDRESS, this->args.value);
    return ERROR::NONE;
}

EnablePumpMotorWriterResult EnablePumpMotorWriter::createResult() {
    return EnablePumpMotorWriterResult();
}

#ifndef _ENABLE_PUMP_MOTOR_WRITER_HPP_
#define _ENABLE_PUMP_MOTOR_WRITER_HPP_

#include "../../setting-writer.hpp"

class EnablePumpMotorWriterArgs {
public:
    bool value;
};

class EnablePumpMotorWriterResult {

};

class EnablePumpMotorWriter : public SettingWriter<EnablePumpMotorWriterResult, EnablePumpMotorWriterArgs> {
public:
    EnablePumpMotorWriter(EnablePumpMotorWriterArgs &args);

protected:
    enum ERROR write() override;
    EnablePumpMotorWriterResult createResult() override;
};

#endif

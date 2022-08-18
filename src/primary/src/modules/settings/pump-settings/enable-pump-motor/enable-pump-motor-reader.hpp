#ifndef _ENABLE_PUMP_MOTOR_READER_HPP_
#define _ENABLE_PUMP_MOTOR_READER_HPP_

#include "../../setting-reader.hpp"

class EnablePumpMotorReaderArgs {

};

class EnablePumpMotorReaderResult {
public:
    bool value;
};

class EnablePumpMotorReader : public SettingReader<EnablePumpMotorReaderResult, EnablePumpMotorReaderArgs> {
public:
    EnablePumpMotorReader(EnablePumpMotorReaderArgs &args);

protected:
    bool value;

protected:
    enum ERROR read() override;
    EnablePumpMotorReaderResult createResult() override;
};

#endif

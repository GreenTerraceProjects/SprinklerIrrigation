#ifndef _ENABLE_PUMP_MOTOR_HPP_
#define _ENABLE_PUMP_MOTOR_HPP_

#include "../../../inputs/input.hpp"

class EnablePumpMotor : public Input {
public:
    EnablePumpMotor();

protected:
    void initialize() override;
    bool handleKeyPressed(char inputKey) override;

protected:
    bool readSetting();
    void writeSetting(bool value);
    void showOptions();
};

#endif

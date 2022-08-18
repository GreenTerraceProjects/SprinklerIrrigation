#ifndef _ENABLE_PUMP_MOTOR_HPP_
#define _ENABLE_PUMP_MOTOR_HPP_

#include "../../../inputs/input.hpp"

class EnablePumpMotorArgs {

};

class EnablePumpMotorResult {

};

class EnablePumpMotor : public Input<EnablePumpMotorResult, EnablePumpMotorArgs> {
public:
    EnablePumpMotor(EnablePumpMotorArgs &args);

protected:
    void initialize() override;
    bool handleInput(char inputKey) override;
    EnablePumpMotorResult createResult() override;

protected:
    bool readSetting();
    void writeSetting(bool value);
    void showOptions();
};

#endif

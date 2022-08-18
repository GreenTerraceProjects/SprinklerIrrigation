#ifndef _SETTING_READER_HPP_
#define _SETTING_READER_HPP_

#include "../errors/errors.hpp"

template <typename TResult, typename TArgs>
class SettingReader {
protected:
    SettingReader(TArgs &args);

protected:
    TArgs &args;
    TResult result;

public:
    enum ERROR run();

protected:
    virtual enum ERROR read() = 0;
    virtual TResult createResult() = 0;

public:
    virtual TResult getResult();
};

template <typename TResult, typename TArgs>
SettingReader<TResult, TArgs>::SettingReader(TArgs &args)
    : args(args) {

}

template <typename TResult, typename TArgs>
enum ERROR SettingReader<TResult, TArgs>::run() {
    enum ERROR error = this->read();

    if (error == ERROR::NONE) {
        this->result = this->createResult();
    }

    return error;
}

template <typename TResult, typename TArgs>
TResult SettingReader<TResult, TArgs>::getResult() {
    return this->result;
}

#endif

#ifndef _SETTING_WRITER_HPP_
#define _SETTING_WRITER_HPP_

#include "../errors/errors.hpp"

template <typename TResult, typename TArgs>
class SettingWriter {
public:
    SettingWriter(TArgs &args);

protected:
    TResult result;
    TArgs &args;

public:
    enum ERROR run();

protected:
    virtual enum ERROR write() = 0;
    virtual TResult createResult() = 0;

public:
    virtual TResult getResult();
};

template <typename TResult, typename TArgs>
SettingWriter<TResult, TArgs>::SettingWriter(TArgs &args)
    : args(args) {

}

template <typename TResult, typename TArgs>
enum ERROR SettingWriter<TResult, TArgs>::run() {
    enum ERROR error = this->write();

    if (error == ERROR::NONE) {
        this->result = this->createResult();
    }

    return error;
}

template <typename TResult, typename TArgs>
TResult SettingWriter<TResult, TArgs>::getResult() {
    return this->result;
}

#endif

/*
template <typename TResult, typename TArgs>
class SettingWriter : public SettingWriterBase<TResult, TArgs> {
protected:
    SettingWriter(TArgs &args);
    virtual EmptyResult createResult() override
};

template <typename TArgs>
class SettingWriter<EmptyResult, TArgs> : public SettingWriterBase<EmptyArgs, TArgs> {
protected:
    SettingWriter(TArgs &args);

protected:
    virtual EmptyResult createResult() override;
};
*/

/*
template <typename TResult, typename TArgs>
SettingWriter<TResult, TArgs>::SettingWriter(TArgs &args)
    : SettingWriterBase(args) {

}

template <typename TArgs>
SettingWriter<EmptyResult, TArgs>::SettingWriter(TArgs &args)
    : SettingWriterBase(args) {
    
}

template <typename TArgs>
EmptyResult SettingWriter<EmptyResult, TArgs>::createResult() {
    return EmptyResult();
}
*/

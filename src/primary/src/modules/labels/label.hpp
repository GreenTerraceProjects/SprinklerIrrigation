#ifndef _LABEL_HPP_
#define _LABEL_HPP_

template <typename TArgs>
class Label {
protected:
    TArgs &args;

protected:
    Label(TArgs &args);

public:
    virtual void view();

protected:
    virtual void initialize() = 0;
    virtual void display() = 0;
};

template <typename TArgs>
Label<TArgs>::Label(TArgs &args)
    : args(args) {

}

template <typename TArgs>
void Label<TArgs>::view() {
    this->initialize();
    this->display();
}

#endif

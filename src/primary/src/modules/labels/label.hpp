#ifndef _LABEL_HPP_
#define _LABEL_HPP_

class Label {
public:
    virtual void view();

protected:
    virtual void initialize() = 0;
    virtual void display() = 0;
};

#endif

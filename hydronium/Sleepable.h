#ifndef SLEEPABLE_H
#define SLEEPABLE_H
class Sleepable {
public:
    virtual void sleep() = 0;
    virtual void wake() = 0;
};
#endif
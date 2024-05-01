#ifndef TIMER_H
#define TIMER_H
#include <chrono>

class SimpleTimer
{
public:
    SimpleTimer();
    virtual ~SimpleTimer();
    void start();
    int get();

protected:

private:
    std::chrono::time_point<std::chrono::steady_clock> startT;
};

#endif // TIMER_H

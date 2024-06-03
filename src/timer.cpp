#include "Timer.h"

SimpleTimer::SimpleTimer()
{
    //ctor
}

SimpleTimer::~SimpleTimer()
{
    //dtor
}

void SimpleTimer::start() {
    startT = std::chrono::steady_clock::now();
}

int SimpleTimer::get() {
    std::chrono::time_point<std::chrono::steady_clock> endT = std::chrono::steady_clock::now();

    std::chrono::microseconds diff = std::chrono::duration_cast<std::chrono::microseconds>(endT - startT);

    return diff.count();
}

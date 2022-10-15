#pragma once
#include <chrono>
class FrameTime
{
public:
    FrameTime()
    {
        last = std::chrono::steady_clock::now();
    }
    float Mark()
    {
        const auto old = last;
        last = std::chrono::steady_clock::now();
        const std::chrono::duration<float> framTime = last - old;
        return framTime.count();
    }

private:
    std::chrono::steady_clock::time_point last;
};
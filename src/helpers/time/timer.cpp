#include "helpers/time/timer.hpp"

namespace helpers::time
{

Timer::Timer() :
    baseTime(std::time(NULL)) // Null to have the number of seconds after the Epoch
{
}

std::time_t Timer::getNumberOfElapsedSeconds() const
{
    std::time_t now = std::time(NULL);

    return now - this->baseTime;
}

} // namespace helpers::time
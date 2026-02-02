#include <optional>

#include "input_generation_sessions/afl_run/fuzzing_timeout.hpp"

namespace input_generation_sessions::afl_run
{

FuzzingTimeout::FuzzingTimeout() :
    timeToFuzz(std::nullopt)
{
}

FuzzingTimeout::FuzzingTimeout(float _timeToFuzz) :
    timeToFuzz(_timeToFuzz)
{
}

FuzzingTimeoutMode FuzzingTimeout::getTimeoutMode() const
{
    FuzzingTimeoutMode result;

    if (this->timeToFuzz == std::nullopt)
    {
        result = FuzzingTimeoutMode::WITHOUT_TIMEOUT;
    }
    else if (this->timeToFuzz.value() == 0)
    {
        result = FuzzingTimeoutMode::NULL_TIMEOUT;
    }
    else
    {
        result = FuzzingTimeoutMode::WITH_TIMEOUT;
    }

    return result;
}

float FuzzingTimeout::getTimeToFuzz() const
{
    float result;

    if ((this->timeToFuzz == std::nullopt) || (this->timeToFuzz.value() < 0))
    {
        result = 0;
    }
    else
    {
        result = this->timeToFuzz.value();
    }

    return result;
}

} // namespace input_generation_sessions::afl_run
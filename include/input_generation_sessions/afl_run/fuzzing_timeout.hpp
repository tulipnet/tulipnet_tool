#ifndef FUZZING_MODE_HPP
#define FUZZING_MODE_HPP

#include <optional>

namespace input_generation_sessions::afl_run
{

enum class FuzzingTimeoutMode
{
    WITHOUT_TIMEOUT,
    WITH_TIMEOUT,
    NULL_TIMEOUT
};

class FuzzingTimeout;

} // namespace input_generation_sessions::afl_run

namespace input_generation_sessions::afl_run
{

class FuzzingTimeout
{
    private:
        std::optional<float> timeToFuzz;

    public:
        FuzzingTimeout();
        FuzzingTimeout(float _timeToFuzz);

        FuzzingTimeoutMode getTimeoutMode() const;
        float getTimeToFuzz() const;
};

} // namespace input_generation_sessions::afl_run

#endif
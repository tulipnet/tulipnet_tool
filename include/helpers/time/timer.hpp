#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>

namespace helpers::time
{
    class Timer;
};

namespace helpers::time
{

/**
 * @brief A simple timer based on the system time
 * 
 */
class Timer
{
    private:
        std::time_t baseTime;

    public:
        /**
         * @brief Construct a new Timer object, with a base time = now.
         * 
         */
        Timer();

        /**
         * @brief Get the Number Of Elapsed Seconds after the creation of the Timer
         * 
         * @return std::time_t The number of elapsed seconds
         */
        std::time_t getNumberOfElapsedSeconds() const;
};

} // namespace helpers::time

#endif
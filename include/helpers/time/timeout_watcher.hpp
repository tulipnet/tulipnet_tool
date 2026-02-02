#ifndef TIMEOUT_WATCHER_HPP
#define TIMEOUT_WATCHER_HPP

#include <exception>

namespace helpers::time
{

class TimeoutWatcherNotLaunchedAsJobException;
class TimeoutWatcher;

} // namespace helpers::time

#include "helpers/runnables/job.hpp"
#include "helpers/time/timer.hpp"

namespace helpers::time
{

/**
 * @brief Thrown when a TimeoutLauncher job task is launched standalone without calling Job::run().
 * 
 */
class TimeoutWatcherNotLaunchedAsJobException : public std::exception
{
    public:
        const char * what() const throw() override;
};

/**
 * @brief A timeout watcher that will send a SIGTERM to its father when the timeout is reached.
 * 
 */
class TimeoutWatcher : public helpers::runnables::Job
{
    private:
        std::time_t timeout;
        Timer timer;

    public:
        /**
         * @brief Construct a new TimeoutWatcher object.
         * 
         * @param _timeout The associated timeout.
         */
        TimeoutWatcher(std::time_t _timeout);

        /**
         * @brief Task associated to the TimeoutWatcher.
         * 
         * @throws TimeoutWatcherNotLaunchedAsJobException If this function is called directly, instead of calling its superclass method Job::run().
         */
        void jobTask() const override;
};

} // namespace helpers::time

#endif
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <unistd.h>

#include "helpers/time/timer.hpp"
#include "helpers/time/timeout_watcher.hpp"
#include "helpers/runnables/job.hpp"

namespace helpers::time
{

#pragma region TimeoutWatcherNotLaunchedAsJobException

const char* TimeoutWatcherNotLaunchedAsJobException::what() const throw()
{
    return "ERROR : The TimeoutWatcher was not launched through Job::run()";
}

#pragma endregion

TimeoutWatcher::TimeoutWatcher(std::time_t _timeout) :
    helpers::runnables::Job(true),
    timeout(_timeout),
    timer()
{
}

void TimeoutWatcher::jobTask() const
{
    std::optional<pid_t> fatherPid = this->getPPid();

    if (fatherPid != std::nullopt)
    {
        while (true)
        {
            if (this->timer.getNumberOfElapsedSeconds() > this->timeout)
            {
                kill(fatherPid.value(), SIGTERM);

                exit(0);
            }
        }
    }
    else
    {
        throw TimeoutWatcherNotLaunchedAsJobException();
    }
}

} // namespace helpers::time
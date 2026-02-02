#ifndef JOBS_MANAGER_HPP
#define JOBS_MANAGER_HPP

#include "helpers/runnables/job.hpp"
#include <map>
#include <optional>

namespace helpers::runnables
{

class JobsManager;

} // namespace helpers::runnables

#include "helpers/runnables/runnables_manager.hpp"

namespace helpers::runnables
{

/**
 * @brief A RunnableManager for the class Job.
 * 
 */
class JobsManager : public RunnablesManager
{
    public:
        /**
         * @brief Construct a new JobsManager object with a vector of Job, and a maximum number of simultaneously ran Job instances.
         * 
         * @param _jobs The vector containing pointers to Job instances to run.
         * @param _maximumNumberOfSimultaneouslyRanRunnables Number of Job that can be ran simultaneously (It is recommended to provide a number <= Number of CPUs of the machine).
         */
        JobsManager(std::vector<Job*> _jobs, int _maximumNumberOfSimultaneouslyRanRunnables);

        /**
         * @brief Construct a new JobsManager object with a vector of Job, a maximum number of simultaneously ran Job instances, and an Observer triggered when each Job is finished.
         * 
         * @param _jobs The vector containing pointers to Job instances to run.
         * @param _maximumNumberOfSimultaneouslyRanRunnables Number of Job that can be ran simultaneously (It is recommended to provide a number <= Number of CPUs of the machine).
         * @param _endOfRunnableObserver The Observer to trigger when one Job is finished
         */
        JobsManager(std::vector<Job*> _jobs, int _maximumNumberOfSimultaneouslyRanRunnables, const helpers::Observer* _endOfRunnableObserver);

        /**
         * @brief Get the result code of all Job instances if they **are all finished**.
         * 
         * @return std::optional<std::map<const Job*, int>> If all Job instances are finished, a mapping from the Job pointer to its status code. If not, `std::nullopt`.
         */
        std::optional<std::map<const Job*, int>> getResultsCode() const;
};

} // namespace helpers::runnables

#endif
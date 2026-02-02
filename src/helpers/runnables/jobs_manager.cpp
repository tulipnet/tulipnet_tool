#include <map>
#include <optional>

#include "helpers/runnables/job.hpp"
#include "helpers/runnables/runnable.hpp"
#include "helpers/runnables/runnables_manager.hpp"
#include "helpers/vector.hpp"
#include "helpers/runnables/jobs_manager.hpp"

namespace helpers::runnables
{

JobsManager::JobsManager(std::vector<Job*> _jobs, int _maximumNumberOfSimultaneouslyRanRunnables) :
    RunnablesManager(helpers::vector::cast<Job*, Runnable*>(_jobs), _maximumNumberOfSimultaneouslyRanRunnables)
{
}

JobsManager::JobsManager(std::vector<Job*> _jobs, int _maximumNumberOfSimultaneouslyRanRunnables, const helpers::Observer* _endOfRunnableObserver) :
    RunnablesManager(helpers::vector::cast<Job*, Runnable*>(_jobs), _maximumNumberOfSimultaneouslyRanRunnables, _endOfRunnableObserver)
{
}

std::optional<std::map<const Job*, int>> JobsManager::getResultsCode() const
{
    std::optional<std::map<const Job*, int>> result;
    Job* currentRunnableAsJob;

    if (this->areAllFinished() == true)
    {
        result = std::optional<std::map<const Job*, int>>(std::map<const Job*, int>());

        for (Runnable* runnable : this->getRunnables())
        {
            currentRunnableAsJob = (Job*)runnable;

            result->insert(std::pair<Job*, int>(currentRunnableAsJob, currentRunnableAsJob->getReturnCode().value())); // Because all Jobs are finished, the optional has a defined value
        }
    }

    return result;
}

} // namespace helpers::runnables
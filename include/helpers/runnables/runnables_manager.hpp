#ifndef RUNNABLES_MANAGER_HPP
#define RUNNABLES_MANAGER_HPP

#include <cstddef>
#include <exception>
#include <vector>

namespace helpers::runnables
{

class RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException;
class RunnablesManager;

} // namespace helpers::runnables

#include "helpers/runnables/runnable.hpp"
#include "helpers/observer.hpp"

namespace helpers::runnables
{

class RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException : public std::exception
{
    public:
        virtual const char* what() const throw() override;
};

/**
 * @brief Collection of Runnable that can be ran in parallel.
 * 
 */
class RunnablesManager
{
    private:
        std::vector<Runnable*> runnables;
        int maximumNumberOfSimultaneouslyRanRunnables;
        const helpers::Observer* endOfRunnableObserver;
        std::size_t countOfRanRunnables;
        bool stoped;
        Runnable** launchedRunnables;

        void initLaunchedRunnables();

    protected:
        /**
         * @brief Get all instances of Runnable contained in this RunnablesManager.
         * 
         * @return const std::vector<Runnable*>& A vector containing pointers to the associated Runnable instances.
         */
        const std::vector<Runnable*>& getRunnables() const;

    public:
        /**
         * @brief Construct a new RunnablesManager object.
         * 
         * @param _runnables Runnables to link to the new RunnablesManager
         * @param _maximumNumberOfSimultaneouslyRanRunnables Number of Runnable that can be ran simultaneously (It is recommended to provide a number <= Number of CPUs of the machine)
         *
         * @throws RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException Thrown when _maximumNumberOfSimultaneouslyRanRunnables == 0.
         */
        RunnablesManager(std::vector<Runnable*> _runnables, int _maximumNumberOfSimultaneouslyRanRunnables);

        /**
         * @brief Construct a new RunnablesManager object with an observer triggered when one Runnable ends.
         * 
         * @param _runnables Runnables to link to the new RunnablesManager
         * @param _maximumNumberOfSimultaneouslyRanRunnables Number of Runnable that can be ran simultaneously (It is recommended to provide a number <= Number of CPUs of the machine)
         * @param _endOfRunnableObserver The Observer to trigger when one Runnable is finished
         *
         * @throws RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException Thrown when _maximumNumberOfSimultaneouslyRanRunnables == 0.
         */
        RunnablesManager(std::vector<Runnable*> _runnables, int _maximumNumberOfSimultaneouslyRanRunnables, const helpers::Observer* _endOfRunnableObserver);

        /**
         * @brief Run all runnables of this RunnablesManager.
         * 
         */
        void runRunnables();

        /**
         * @brief Are all the Runnable instances finished ?
         * 
         * @return true All of them are finished
         * @return false At least one of them is not finished
         */
        bool areAllFinished() const;

        /**
         * @brief Stop all Runnable instances by calling `runnables[i]->end();`.
         * 
         */
        void stopRunnables();
};

} // namespace helpers::runnables

#endif
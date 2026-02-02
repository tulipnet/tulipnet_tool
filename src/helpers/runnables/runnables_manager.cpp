#include <algorithm>
#include <stack>
#include <vector>

#ifdef DEBUG
#include <iostream>
#endif

#include "helpers/runnables/runnables_manager.hpp"
#include "helpers/runnables/runnable.hpp"

namespace helpers::runnables
{

#pragma region RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException

const char* RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException::what() const throw()
{
    return "ERROR : The maximum number of simultaneously ran runnables is null";
}

#pragma endregion

void RunnablesManager::initLaunchedRunnables()
{
    int i;

    for (i = 0; i < this->maximumNumberOfSimultaneouslyRanRunnables; ++i)
    {
        this->launchedRunnables[i] = nullptr;
    }
}

const std::vector<Runnable*>& RunnablesManager::getRunnables() const
{
    return this->runnables;
}

RunnablesManager::RunnablesManager(std::vector<Runnable*> _runnables, int _maximumNumberOfSimultaneouslyRanRunnables) :
    runnables(_runnables),
    maximumNumberOfSimultaneouslyRanRunnables(_maximumNumberOfSimultaneouslyRanRunnables),
    endOfRunnableObserver(nullptr),
    countOfRanRunnables(0),
    stoped(false),
    launchedRunnables(new Runnable*[this->maximumNumberOfSimultaneouslyRanRunnables])
{
    this->initLaunchedRunnables();

    if (this->maximumNumberOfSimultaneouslyRanRunnables == 0)
    {
        throw new RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException();
    }
}

RunnablesManager::RunnablesManager(std::vector<Runnable*> _runnables, int _maximumNumberOfSimultaneouslyRanRunnables, const Observer* _endOfRunnableObserver) :
    runnables(_runnables),
    maximumNumberOfSimultaneouslyRanRunnables(_maximumNumberOfSimultaneouslyRanRunnables),
    endOfRunnableObserver(_endOfRunnableObserver),
    countOfRanRunnables(0),
    stoped(false),
    launchedRunnables(new Runnable*[this->maximumNumberOfSimultaneouslyRanRunnables])
{
    this->initLaunchedRunnables();

    if (this->maximumNumberOfSimultaneouslyRanRunnables == 0)
    {
        throw new RunnablesManagerMaximumNumberOfSimultaneouslyRanRunnablesIsNullException();
    }
}

void RunnablesManager::runRunnables()
{
    std::stack<Runnable*, std::vector<Runnable*>> remainingRunnables(this->runnables);
    int i;
    long j;
    int numberOfRunnables = this->runnables.size();
    int numberOfIterations = (numberOfRunnables / this->maximumNumberOfSimultaneouslyRanRunnables) + 1;
    int countOfRunnables = 0;

    this->stoped = false;

    for (i = 0; i < numberOfIterations; ++i)
    {
        j = 0;

        while ((j < this->maximumNumberOfSimultaneouslyRanRunnables) && (countOfRunnables < numberOfRunnables))
        {
            this->launchedRunnables[j] = remainingRunnables.top();

            remainingRunnables.pop();

            this->launchedRunnables[j]->run();

            #ifdef DEBUG

            std::cout << "Launched runnable : " << std::hex << this->launchedRunnables[j] << std::endl;

            #endif

            ++countOfRunnables;
            ++j;
        }

        j = 0;

        while (j < std::min(numberOfRunnables - (i * this->maximumNumberOfSimultaneouslyRanRunnables), this->maximumNumberOfSimultaneouslyRanRunnables))
        {
            #ifdef DEBUG

            std::cout << "Waiting runnable : " << std::hex << this->launchedRunnables[j] << std::endl;

            #endif

            if (this->stoped == false)
            {
                this->launchedRunnables[j]->wait();

                this->launchedRunnables[j] = nullptr;
            }

            #ifdef DEBUG

            if (this->stoped == false)
            {
                std::cout << "Waited runnable : " << std::hex << this->launchedRunnables[j] << std::endl;
            }

            #endif

            if ((this->stoped == false) && (this->endOfRunnableObserver != nullptr))
            {
                this->endOfRunnableObserver->notify();
            }

            ++this->countOfRanRunnables;
            ++j;
        }
    }

    if (this->stoped == false) // Double free without this :)
    {
        delete[] this->launchedRunnables;
    }
}

bool RunnablesManager::areAllFinished() const
{
    std::size_t numberOfRunnables = this->runnables.size();
    bool result;

    if ((this->stoped == true) || (numberOfRunnables == this->countOfRanRunnables))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

void RunnablesManager::stopRunnables()
{
    int i;

    this->stoped = true;
    this->runnables.clear();

    for (i = 0; i < this->maximumNumberOfSimultaneouslyRanRunnables; ++i)
    {
        if (this->launchedRunnables[i] != nullptr)
        {
            #ifdef DEBUG

            std::cout << "Ended runnable : " << std::hex << this->launchedRunnables[i] << std::endl;

            #endif

            this->launchedRunnables[i]->end();
        }
    }

    delete[] this->launchedRunnables;
}

} // namespace helpers::runnables
#ifndef OBSERVER_HPP
#define OBSERVER_HPP

namespace helpers
{

/**
 * @brief A basic observer with only one `notify()` function
 * 
 */
class Observer
{
    public:
        /**
         * @brief Function to run when this observer is triggered
         * 
         */
        virtual void notify() const = 0;
};

} // namespace helpers

#endif
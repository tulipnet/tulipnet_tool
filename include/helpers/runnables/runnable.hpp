#ifndef RUNNABLE_HPP
#define RUNNABLE_HPP

namespace helpers::runnables
{

class Runnable;

/**
 * @brief Base abstract class for a Runnable object. A runnable is a thing that can be ran, waited and exited. It is particularly suitable for managing fork() etc.
 * 
 */
class Runnable
{
    public:
        /**
         * @brief Run the Runnable
         * 
         */
        virtual void run() = 0;

        /**
         * @brief Wait the Runnable ends (And eventually set a status code)
         * 
         */
        virtual void wait() = 0;

        /**
         * @brief End the Runnable by ending it
         * 
         */
        virtual void end() = 0;

        virtual ~Runnable() = 0;
};

} // namespace helpers::runnables

#endif
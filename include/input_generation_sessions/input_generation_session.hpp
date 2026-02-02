#ifndef INPUT_GENERATION_SESSION_HPP
#define INPUT_GENERATION_SESSION_HPP

#include <string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/stat.h>
#include <exception>
#include <fcntl.h>

namespace input_generation_sessions
{

class InputGenerationSessionNotEmptyResultDirectoryException;
class InputGenerationSessionRequiredFileDoesNotExistException;
class InputGenerationSessionAlreadyExecutedException;
class InputGenerationSession;

} // namespace input_generation_sessions

#include "helpers/runnables/runnable.hpp"

namespace input_generation_sessions
{

#pragma region Exceptions

/**
 * @brief Thrown when the result directory of the current InputGenerationSession is not empty.
 * 
 */
class InputGenerationSessionNotEmptyResultDirectoryException : public std::exception
{
    private:
        std::string errorMessage;

    public:
        InputGenerationSessionNotEmptyResultDirectoryException(std::string _directory);

        virtual const char* what() const throw();
};

/**
 * @brief A required file does not exist.
 * 
 */
class InputGenerationSessionRequiredFileDoesNotExistException : public std::exception
{
    private:
        std::string errorMessage;

    public:
        InputGenerationSessionRequiredFileDoesNotExistException(std::string _filePath);

        virtual const char* what() const throw();
};

/**
 * @brief An InputGenerationSession was already executed.
 * 
 */
class InputGenerationSessionAlreadyExecutedException : public std::exception
{
    public:
        std::string errorMessage;

    public:
        InputGenerationSessionAlreadyExecutedException(std::string _directory);

        virtual const char* what() const throw();
};

#pragma endregion

/**
 * @brief A class modeling an input generation session. An input generation session generates and prepare inputs before the oracle stage.
 * 
 */
class InputGenerationSession : public helpers::runnables::Runnable
{
    private:
        std::string binaryToTestPath;
        std::vector<std::string> binaryToTestArgs;
        std::string resultsPath;
        std::string generatedSeedsPath;
        int id;
        int cpuCost;
        bool launched;

    protected:
        static void checkIfFilePathExists(std::string filePath);

        void setLaunched();

    public:
        /**
         * @brief Construct a new Input Generation Session object
         * 
         * @param _binaryToTestPath Path to the binary to test to generate inputs
         * @param _binaryToTestArgs Arguments passed to the binary to test
         * @param _resultsPath Path to the results
         * @param _generatedSeedsPath Path to the seeds used by the underlying tool
         * @param _id Id of the session
         * @param _cpuCost Number of CPUs used by this Input Generation Session
         */
        InputGenerationSession(std::string _binaryToTestPath, std::vector<std::string> _binaryToTestArgs, std::string _resultsPath, std::string _generatedSeedsPath, int _id, int _cpuCost);

        virtual bool wasAlreadyExecuted() const = 0;
        virtual bool isFinished() const = 0;

        // Getters
        std::string getResultsPath() const;
        std::string getBinaryToTestPath() const;
        std::vector<std::string> getBinaryToTestArgs() const;
        std::string getGeneratedSeedsPath() const;
        int getId() const;
        int getCPUCost() const;
        bool wasLaunched() const;

        // Setters
        void setGeneratedSeedsPath(std::string _generatedSeedsPath);
};

} // namespace input_generation_sessions

#endif
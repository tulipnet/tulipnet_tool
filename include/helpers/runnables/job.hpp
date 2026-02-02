#ifndef JOB_HPP
#define JOB_HPP

#include <sched.h>
#include <string>
#include <exception>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <optional>

namespace helpers::runnables
{

class JobUnableToForkException;
class JobUnableToOpenTheStdinFileException;
class JobUniqueExecutionViolationException;
class Job;

}

#include "helpers/runnables/runnable.hpp"

namespace helpers::runnables
{

#pragma region Exceptions

/**
 * @brief Exception thrown when a Job cannot do a fork().
 * 
 */
class JobUnableToForkException : public std::exception
{
    public:
        JobUnableToForkException();

        virtual const char* what() const throw();
};

/**
 * @brief Exception thrown when a Job with an attached stdin file cannot open it.
 * 
 */
class JobUnableToOpenTheStdinFileException : public std::exception
{
    private:
        std::string message;

    public:
        JobUnableToOpenTheStdinFileException(std::string _filePath);

        virtual const char* what() const throw();
};

/**
 * @brief Exception thrown when an unique Job was attempted to be ran more than one time.
 * 
 */
class JobUniqueExecutionViolationException : public std::exception
{
    public:
        JobUniqueExecutionViolationException();

        virtual const char* what() const throw();
};

#pragma endregion

/**
 * @brief A job is a parallel implementation of a Runnable based on fork(). The associated routine has to be written by overriding jobTask(). The job can have dedicated inputs/outputs (stdin/stdout/stderr). A job can be unique, so if it is, it can be launched only one time.
 * 
 */
class Job : public Runnable
{
    private:
        std::string stdoutFilePath;
        std::string stderrFilePath;
        std::string stdinFilePath;
        bool usesAStdoutAndAStderrFiles;
        bool usesAStdinFile;
        pid_t pid;
        bool wasLaunched;
        bool isFinished;
        bool isUnique;
        std::optional<int> returnCode;
        std::optional<pid_t> ppid;

    protected:
        virtual void jobTask() const = 0;

    public:
        /**
         * @brief Construct a new Job object without dedicated inputs/outputs
         * 
         */
        Job();

        /**
         * @brief Construct a new Job object that can be unique.
         * 
         * @param _isUnique true if unique, false if not.
         */
        Job(bool _isUnique);

        /**
         * @brief Construct a new Job object with dedicated stdout and stderr. Files are automatically created when running it.
         * 
         * @param _stdoutFilePath Path to a file to use for stdout
         * @param _stderrFilePath Path to a file to use for stderr
         */
        Job(std::string _stdoutFilePath, std::string _stderrFilePath);

        /**
         * @brief Construct a new Job object that can be unique, and with dedicated stdout and stderr. Files are automatically created when running it.
         * 
         * @param _isUnique true if unique, false if not.
         * @param _stdoutFilePath Path to a file to use for stdout
         * @param _stderrFilePath Path to a file to use for stderr
         */
        Job(bool _isUnique, std::string _stdoutFilePath, std::string _stderrFilePath);

        /**
         * @brief Construct a new Job object with dedicated stdin, stdout and stderr. Files related to stdout and stderr are automatically created when running it. The file related to stdin must exist before creating the object.
         * 
         * @param _stdoutFilePath Path to a file to use for stdout
         * @param _stderrFilePath Path to a file to use for stderr
         * @param _stdinFilePath Path to a file to use for stdin
         */
        Job(std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath);

        /**
         * @brief Construct a new Job object that can be unique, with dedicated stdin, stdout and stderr. Files related to stdout and stderr are automatically created when running it. The file related to stdin must exist before creating the object.
         * 
         * @param _isUnique true if unique, false if not.
         * @param _stdoutFilePath Path to a file to use for stdout
         * @param _stderrFilePath Path to a file to use for stderr
         * @param _stdinFilePath Path to a file to use for stdin
         */
        Job(bool _isUnique, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath);

        /**
         * @brief Run this Job
         * 
         * @throws JobUnableToForkException It was impossible to fork() to run the Job
         * @throws JobUnableToOpenTheStdinFileException The provided stdin file to the constructor is unable to be opened (Maybe it does not exist, or something else...).
         * @throws JobUniqueExecutionViolationException It was tried to run more than one time this unique Job.
         */
        virtual void run() override final;

        /**
         * @brief Wait this Job by using waitpid().
         * 
         */
        virtual void wait() override final;

        /**
         * @brief Kill this Job with a SIGINT by using kill(), and then this->wait() it.
         * 
         */
        virtual void end() override final;

        #pragma region Getters

        /**
         * @brief Was this Job launched ?
         * 
         * @return true This Job was launched
         * @return false This Job was not launched
         */
        bool getWasLaunched() const;

        /**
         * @brief Is this Job finished ?
         * 
         * @return true This Job is finished
         * @return false This Job is not finished
         */
        bool getIsFinished() const;

        /**
         * @brief Get the stdout file path
         * 
         * @return std::optional<std::string> An optional containing the stdout file path if provided to a constructor. std::nullopt otherwise.
         */
        std::optional<std::string> getStdoutFilePath() const;

        /**
         * @brief Get the stderr file path
         * 
         * @return std::optional<std::string> An optional containing the stderr file path if provided to a constructor. std::nullopt otherwise.
         */
        std::optional<std::string> getStderrFilePath() const;

        /**
         * @brief Get the result code of this finished and waited (By this->wait()) Job.
         * 
         * @return std::optional<int> An optional containing the result code of this job if it was finished and waited by (this->wait()). std::nullopt otherwise.
         */
        std::optional<int> getReturnCode() const;

        /**
         * @brief Get the pid associated to this Job.
         * 
         * @return pid_t The pid associated to this Job.
         */
        pid_t getPid() const;

        /**
         * @brief Returns the PPID (Parent PID) of this Job if applicable.
         * 
         * @return std::optional<pid_t> If the current process is the children, the PPID of it. If the current process is the father, std::nullopt.
         */
        std::optional<pid_t> getPPid() const;

        #pragma endregion
};

} // namespace helpers::runnables

#endif
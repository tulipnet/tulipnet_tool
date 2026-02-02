#include <csignal>
#include <optional>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

#ifdef DEBUG
#include <iostream>
#endif

#include "helpers/runnables/job.hpp"

namespace helpers::runnables
{

#pragma region JobUnableToForkException

JobUnableToForkException::JobUnableToForkException()
{
}

const char* JobUnableToForkException::what() const throw()
{
    return "ERROR : Unable to fork.";
}

#pragma endregion
#pragma region JobUnableToOpenTheStdinFileException

JobUnableToOpenTheStdinFileException::JobUnableToOpenTheStdinFileException(std::string _filePath) :
    message("ERROR : Unable to open the file \"" + _filePath + "\" as a stdin in read only.")
{
}

const char* JobUnableToOpenTheStdinFileException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion
#pragma region JobUniqueExecutionViolationException

JobUniqueExecutionViolationException::JobUniqueExecutionViolationException()
{
}

const char* JobUniqueExecutionViolationException::what() const throw()
{
    return "ERROR : A unique job execution violation was detected.";
}

#pragma endregion

Job::Job() :
    usesAStdoutAndAStderrFiles(false),
    usesAStdinFile(false),
    pid(-1),
    wasLaunched(false),
    isFinished(false),
    isUnique(false),
    returnCode(std::nullopt),
    ppid(std::nullopt)
{
}

Job::Job(bool _isUnique) :
    Job()
{
    this->isUnique = _isUnique;
}

Job::Job(std::string _stdoutFilePath, std::string _stderrFilePath) :
    Job()
{
    this->stdoutFilePath = _stdoutFilePath;
    this->stderrFilePath = _stderrFilePath;
    this->usesAStdoutAndAStderrFiles = true;
}

Job::Job(bool _isUnique, std::string _stdoutFilePath, std::string _stderrFilePath) :
    Job(_stdoutFilePath, _stderrFilePath)
{
    this->isUnique = _isUnique;
}

Job::Job(std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath) :
    Job(_stdoutFilePath, _stderrFilePath)
{
    this->stdinFilePath = _stdinFilePath;
    this->usesAStdinFile = true;
}

Job::Job(bool _isUnique, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath) :
    Job(_stdoutFilePath, _stderrFilePath, _stdinFilePath)
{
    this->isUnique = _isUnique;
}

void Job::run()
{
    int stdoutFileFD;
    int stderrFileFD;
    int stdinFileFD;
    mode_t fileMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // 0o644 -rw-r--r--

    if (((this->isUnique == true) && (this->wasLaunched == false)) || (this->isUnique == false))
    {
        this->wasLaunched = true;
        this->pid = fork();

        if (this->pid == -1)
        {
            throw JobUnableToForkException();
        }
        else if (this->pid == 0) // Children
        {
            this->ppid = getppid();

            if (this->usesAStdoutAndAStderrFiles == true)
            {
                stdoutFileFD = open(this->stdoutFilePath.c_str(), O_CREAT | O_WRONLY, fileMode);
                stderrFileFD = open(this->stderrFilePath.c_str(), O_CREAT | O_WRONLY, fileMode);

                dup2(stdoutFileFD, 1); // 1 : stdout
                dup2(stderrFileFD, 2); // 2 : stderr
            }

            if (this->usesAStdinFile == true)
            {
                stdinFileFD = open(this->stdinFilePath.c_str(), O_RDONLY);

                if (stdinFileFD == -1)
                {
                    throw JobUnableToOpenTheStdinFileException(this->stdinFilePath);
                }

                dup2(stdinFileFD, 0); // 0 : stdin
            }

            this->jobTask();

            if (this->usesAStdoutAndAStderrFiles == true)
            {
                close(stdoutFileFD);
                close(stderrFileFD);
            }

            if (this->usesAStdinFile == true)
            {
                close(stdinFileFD);
            }

            exit(0);
        }
        else // Father
        {
            #ifdef DEBUG

            std::cout << std::dec << "PID : " << this->pid << std::endl;

            #endif

            this->wasLaunched = true;
        }
    }
    else
    {
        throw JobUniqueExecutionViolationException();
    }
}

void Job::wait()
{
    int returnedStatus;

    if (this->wasLaunched == true)
    {
        #ifdef DEBUG

        std::cout << std::dec << "Waiting PID " << this->pid << std::endl;

        #endif

        waitpid(this->pid, &returnedStatus, 0);

        this->returnCode = std::optional<int>(returnedStatus);
        this->isFinished = true;
    }
}

void Job::end()
{
    if (this->wasLaunched == true)
    {
        #ifdef DEBUG

        std::cout << "SIGINTing " << std::dec << this->pid << std::endl;

        #endif

        kill(this->pid, SIGINT); // CTRL + C

        this->wait();
    }
}

bool Job::getWasLaunched() const
{
    return this->wasLaunched;
}

bool Job::getIsFinished() const
{
    return this->isFinished;
}

std::optional<std::string> Job::getStdoutFilePath() const
{
    std::optional<std::string> result;

    if (this->usesAStdoutAndAStderrFiles == true)
    {
        result = this->stdoutFilePath;
    }
    else
    {
        result = std::nullopt;
    }

    return result;
}

std::optional<std::string> Job::getStderrFilePath() const
{
    std::optional<std::string> result;

    if (this->usesAStdoutAndAStderrFiles == true)
    {
        result = this->stderrFilePath;
    }
    else
    {
        result = std::nullopt;
    }

    return result;
}

std::optional<int> Job::getReturnCode() const
{
    return this->returnCode;
}

pid_t Job::getPid() const
{
    return this->pid;
}

std::optional<pid_t> Job::getPPid() const
{
    return this->ppid;
}

} // namespace helpers::runnables
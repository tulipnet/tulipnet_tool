#ifndef EXTERNAL_BINARY_EXECUTION_HPP
#define EXTERNAL_BINARY_EXECUTION_HPP

#include <exception>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <map>
#include <cstring>

namespace helpers::runnables
{

class ExternalBinaryExecutionExecutableFileErrorException;
class ExternalBinaryExecution;

} // namespace helpers::runnables

#include "job.hpp"

namespace helpers::runnables
{

#pragma region Exceptions

/**
 * @brief Exception thrown when there is an error with the executable file provided to the constructor of ExternalBinaryExecution
 * 
 */
class ExternalBinaryExecutionExecutableFileErrorException : public std::exception
{
    private:
        std::string message;

    public:
        ExternalBinaryExecutionExecutableFileErrorException(std::string _executableFilePath);
        ExternalBinaryExecutionExecutableFileErrorException(std::string _executableFilePath, std::string _error);

        virtual const char* what() const throw();
};

#pragma endregion

/**
 * @brief A Job corresponding to executing an external binary (Like `/bin/bash` for example).
 * 
 */
class ExternalBinaryExecution : public Job
{
    private:
        std::string executablePath;
        std::vector<std::string> executableArgs;
        std::map<std::string, std::string> environment;
        bool hasEnvironment;

        static void checkToolPath(std::string toolPath);

        void build(std::string _executablePath, std::vector<std::string> _executableArgs);

    protected:
        virtual void jobTask() const override;

    public:
        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _isUnique Is the associated Job unique ?
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         * @param _stdinFilePath Path to a file containing its stdin
         * @param _environment A map from an environment variable name to its value.
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath, std::map<std::string, std::string> _environment);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _isUnique Is the associated Job unique ?
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         * @param _environment A map from an environment variable name to its value.
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::map<std::string, std::string> _environment);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _isUnique Is the associated Job unique ?
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         * @param _stdinFilePath Path to a file containing its stdin
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _isUnique Is the associated Job unique ?
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _isUnique Is the associated Job unique ?
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _environment A map from an environment variable name to its value.
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::map<std::string, std::string> _environment);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _isUnique Is the associated Job unique ?
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         * @param _stdinFilePath Path to a file containing its stdin
         * @param _environment A map from an environment variable name to its value.
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath, std::map<std::string, std::string> _environment);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         * @param _environment  A map from an environment variable name to its value.
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::map<std::string, std::string> _environment);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         * @param _stdinFilePath Path to a file containing its stdin
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _stdoutFilePath Path to a file to save its stdout
         * @param _stderrFilePath Path to a file to save its stderr
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         * @param _environment  A map from an environment variable name to its value.
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::map<std::string, std::string> _environment);

        /**
         * @brief Construct a new ExternalBinaryExecution object.
         * 
         * @param _executablePath Path to the executable binary to run.
         * @param _executableArgs Arguments to pass to the binary to run in form of a `std::string` vector
         *
         * @throws ExternalBinaryExecutionExecutableFileErrorException If there is a problem with the provided path in `_executablePath` (Like a file that does not exist, or a non-executable file).
         */
        ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs);

        /**
         * @brief Get the name of the associated executable (Without its path).
         * 
         * @return std::string The name of the associated executable without its path.
         */
        std::string getExecutableName() const;

        /**
         * @brief Get the whole bash command to run this ExternalBinaryExecution in a raw Bash shell.
         * 
         * @return std::string The bash command to run this ExternalBinaryExecution in a raw Bash shell.
         */
        std::string getThisAsBashCommand() const;

        /**
         * @brief Add an environment variable to the vector of provided environment variables.
         * 
         * @param name The name of the new environment variable.
         * @param value The value of the new environment variable.
         */
        void addEnvironmentVariable(std::string name, std::string value);

        virtual ~ExternalBinaryExecution();
};

} // namespace helpers::runnables

#endif
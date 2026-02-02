#ifndef BASH_SCRIPT_EXECUTION_HPP
#define BASH_SCRIPT_EXECUTION_HPP

#include <map>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>

namespace helpers::runnables
{

class BashScriptExecution;

} // namespace helpers::runnables

#include "external_binary_execution.hpp"
#include "helpers/runnables/runnable.hpp"

namespace helpers::runnables
{

/**
 * @brief A specific job intended to run a Bash script
 * 
 */
class BashScriptExecution : public Runnable
{
    private:
        std::string scriptPath;
        std::string scriptArgsStr;
        std::map<std::string, std::string> scriptEnvironment;
        std::string stdoutFilePath;
        std::string stderrFilePath;
        ExternalBinaryExecution* execution;

        std::vector<std::string> getBashArgs() const;
        std::map<std::string, std::string> getBashEnvironment() const;

    public:
        /**
         * @brief Construct a new BashScriptExecution object by providing a path to a Bash script.
         * 
         * @param _scriptPath Path to the Bash script.
         */
        BashScriptExecution(std::string _scriptPath);

        /**
         * @brief Construct a new BashScriptExecution object by providing a path to a Bash script, and its arguments.
         * 
         * @param _scriptPath Path to the Bash script.
         * @param _scriptArgsStr Its arguments in form of a string with spaces to separate the arguments (Like in a classical shell invocation).
         */
        BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr);

        /**
         * @brief Construct a new BashScriptExecution object by providing a path to a Bash script, its arguments, and a specific environment
         * 
         * @param _scriptPath Path to the Bash script.
         * @param _scriptArgsStr Its arguments in form of a string with spaces to separate the arguments (Like in a classical shell invocation).
         * @param _environment A map from an environment variable name to its value.
         */
        BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr, std::map<std::string, std::string> _environment);

        /**
         * @brief Construct a new BashScriptExecution object by providing a path to a Bash script, its arguments, and files to save its stout and stderr.
         * 
         * @param _scriptPath Path to the Bash script.
         * @param _scriptArgsStr Its arguments in form of a string with spaces to separate the arguments (Like in a classical shell invocation).
         * @param _stdoutFilePath Path to a file containing its stdout
         * @param _stdoutFilePath Path to a file containing its stderr
         */
        BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr, std::string _stdoutFilePath, std::string _stderrFilePath);

        /**
         * @brief Construct a new BashScriptExecution object by providing a path to a Bash script, its arguments, and files to save its stout and stderr.
         * 
         * @param _scriptPath Path to the Bash script.
         * @param _scriptArgsStr Its arguments in form of a string with spaces to separate the arguments (Like in a classical shell invocation).
         * @param _environment A map from an environment variable name to its value.
         * @param _stdoutFilePath Path to a file containing its stdout
         * @param _stdoutFilePath Path to a file containing its stderr
         */
        BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr, std::map<std::string, std::string> _environment, std::string _stdoutFilePath, std::string _stderrFilePath);

        /**
         * @brief Run this BashScriptExecution
         * 
         */
        virtual void run() override;

        /**
         * @brief Wait this BashScriptExecution
         * 
         */
        virtual void wait() override;

        /**
         * @brief End this BashScriptExecution
         * 
         */
        virtual void end() override;

        /**
         * @brief Get the whole command to execute this script in a raw shell.
         * 
         * @return std::string The whole command to execute this script in a raw shell.
         */
        std::string getThisAsBashCommand() const;

        ~BashScriptExecution();
};

} // namespace helpers::runnables

#endif
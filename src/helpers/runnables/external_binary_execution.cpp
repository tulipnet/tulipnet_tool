#include <optional>
#include <sstream>

#include "helpers/runnables/external_binary_execution.hpp"

namespace helpers::runnables
{

#pragma region ExternalBinaryExecutionExecutableFileErrorException

ExternalBinaryExecutionExecutableFileErrorException::ExternalBinaryExecutionExecutableFileErrorException(std::string _executableFilePath)
{
    message = ("ERROR : There is an error with the file \"" + _executableFilePath + "\". Maybe it does not exists, or it is not executable.");
}

ExternalBinaryExecutionExecutableFileErrorException::ExternalBinaryExecutionExecutableFileErrorException(std::string _executableFilePath, std::string _error)
{
    message = ("ERROR : The file \"" + _executableFilePath + "\" cannot be run. Reason : \"" + _error + "\".");
}

const char* ExternalBinaryExecutionExecutableFileErrorException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion

void ExternalBinaryExecution::checkToolPath(std::string toolPath)
{
    struct stat currentStat;
    int currentResult;
    mode_t rxForOwner = S_IRUSR | S_IXUSR; // 0o0500

    // Check if the file exists
    currentResult = stat(toolPath.c_str(), &currentStat);

    if (currentResult == 0)
    {
        // Check if the file is executable for its owner
        if ((currentStat.st_mode & rxForOwner) != rxForOwner)
        {
            throw ExternalBinaryExecutionExecutableFileErrorException(toolPath, "Not readable and/or executable for its owner");
        }
    }
    else
    {
        throw ExternalBinaryExecutionExecutableFileErrorException(toolPath);
    }
}

void ExternalBinaryExecution::build(std::string _executablePath, std::vector<std::string> _executableArgs)
{
    checkToolPath(_executablePath);

    this->executablePath = _executablePath;
    this->executableArgs = _executableArgs;
}

void ExternalBinaryExecution::jobTask() const
{
    std::size_t executableArgsSize = this->executableArgs.size();
    char** argsAsCArray;
    std::string executableName = this->getExecutableName();
    std::size_t i;
    std::size_t argsAsCArraySize = executableArgsSize + 2; // +2 for the executable name itself, and the final NULL
    char** environmentAsCArray;
    std::size_t environmentSize = this->environment.size();
    std::size_t environmentAsCArraySize = environmentSize + 1; // +1 for the final NULL
    std::map<std::string, std::string>::const_iterator environmentIterator;
    std::string currentEnvironmentVariable;

    argsAsCArray = new char*[argsAsCArraySize];

    argsAsCArray[0] = new char[executableName.size() + 1];
    std::strcpy(argsAsCArray[0], executableName.c_str());

    for (i = 0; i < executableArgsSize; ++i)
    {
        const std::string& currentArg = this->executableArgs.at(i);

        argsAsCArray[i + 1] = new char[currentArg.size() + 1];
        std::strcpy(argsAsCArray[i + 1], currentArg.c_str());
    }

    argsAsCArray[argsAsCArraySize - 1] = (char*)NULL;

    if (this->hasEnvironment == true)
    {
        environmentAsCArray = new char*[environmentAsCArraySize];
        i = 0;

        for (environmentIterator = this->environment.begin(); environmentIterator != this->environment.end(); ++environmentIterator)
        {
            const std::string& environmentVariableName = environmentIterator->first;
            const std::string& environmentVariableValue = environmentIterator->second;

            currentEnvironmentVariable = environmentVariableName + "=" + environmentVariableValue;

            environmentAsCArray[i] = new char[currentEnvironmentVariable.size() + 1];
            std::strcpy(environmentAsCArray[i], currentEnvironmentVariable.c_str());

            ++i;
        }

        environmentAsCArray[environmentAsCArraySize - 1] = (char*)NULL;

        execvpe(this->executablePath.c_str(), argsAsCArray, environmentAsCArray);

        for (i = 0; i < environmentAsCArraySize; ++i)
        {
            delete[] environmentAsCArray[i];
        }

        delete[] environmentAsCArray;
    }
    else
    {
        execvp(this->executablePath.c_str(), argsAsCArray);
    }

    for (i = 0; i < argsAsCArraySize; ++i)
    {
        delete[] argsAsCArray[i];
    }

    delete[] argsAsCArray;
}

ExternalBinaryExecution::ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath, std::map<std::string, std::string> _environment) :
    Job(_isUnique, _stdoutFilePath, _stderrFilePath, _stdinFilePath),
    environment(_environment),
    hasEnvironment(true)
{
    this->build(_executablePath, _executableArgs);
}

ExternalBinaryExecution::ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::map<std::string, std::string> _environment) :
    Job(_isUnique, _stdoutFilePath, _stderrFilePath),
    environment(_environment),
    hasEnvironment(true)
{
    this->build(_executablePath, _executableArgs);
}


ExternalBinaryExecution::ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath) :
    Job(_isUnique, _stdoutFilePath, _stderrFilePath, _stdinFilePath),
    hasEnvironment(false)
{
    this->build(_executablePath, _executableArgs);
}


ExternalBinaryExecution::ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath) :
    Job(_isUnique, _stdoutFilePath, _stderrFilePath),
    hasEnvironment(false)
{
    this->build(_executablePath, _executableArgs);
}

ExternalBinaryExecution::ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs, std::map<std::string, std::string> _environment) :
    Job(_isUnique),
    environment(_environment),
    hasEnvironment(true)
{
    this->build(_executablePath, _executableArgs);
}


ExternalBinaryExecution::ExternalBinaryExecution(bool _isUnique, std::string _executablePath, std::vector<std::string> _executableArgs) :
    Job(_isUnique),
    hasEnvironment(false)
{
    this->build(_executablePath, _executableArgs);
}

ExternalBinaryExecution::ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath, std::map<std::string, std::string> _environment) :
    ExternalBinaryExecution(false, _executablePath, _executableArgs, _stdoutFilePath, _stderrFilePath, _stdinFilePath, _environment)
{
}

ExternalBinaryExecution::ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::map<std::string, std::string> _environment) :
    ExternalBinaryExecution(false, _executablePath, _executableArgs, _stdoutFilePath, _stderrFilePath, _environment)
{
}

ExternalBinaryExecution::ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath, std::string _stdinFilePath) :
    ExternalBinaryExecution(false, _executablePath, _executableArgs, _stdoutFilePath, _stderrFilePath, _stdinFilePath)
{
}

ExternalBinaryExecution::ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::string _stdoutFilePath, std::string _stderrFilePath) :
    ExternalBinaryExecution(false, _executablePath, _executableArgs, _stdoutFilePath, _stderrFilePath)
{
}

ExternalBinaryExecution::ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs, std::map<std::string, std::string> _environment) :
    ExternalBinaryExecution(false, _executablePath, _executableArgs, _environment)
{
}

ExternalBinaryExecution::ExternalBinaryExecution(std::string _executablePath, std::vector<std::string> _executableArgs) :
    ExternalBinaryExecution(false, _executablePath, _executableArgs)
{
}

std::string ExternalBinaryExecution::getExecutableName() const
{
    std::stringstream executablePathStream(this->executablePath);
    std::string currentExecutablePathSection;

    // We keep only the last section (Ex : "/home/totoro/test.exe" => "test.exe")
    while (std::getline(executablePathStream, currentExecutablePathSection, '/'))
    {
    }

    return currentExecutablePathSection;
}

std::string ExternalBinaryExecution::getThisAsBashCommand() const
{
    std::string result = this->executablePath;
    std::optional<std::string> optionalStdoutFilePath = this->getStdoutFilePath();
    std::optional<std::string> optionalStderrFilePath = this->getStderrFilePath();

    for (std::string arg : this->executableArgs)
    {
        result += " " + arg;
    }

    if (optionalStdoutFilePath.has_value() == true)
    {
        result += " > " + optionalStdoutFilePath.value();
    }

    if (optionalStderrFilePath.has_value() == true)
    {
        result += " 2> " + optionalStderrFilePath.value();
    }

    return result;
}

void ExternalBinaryExecution::addEnvironmentVariable(std::string name, std::string value)
{
    if (this->environment.count(name) > 0)
    {
        this->environment.erase(name);
    }

    this->environment.insert(std::pair<std::string, std::string>(name, value));
}

ExternalBinaryExecution::~ExternalBinaryExecution()
{
}

} // namespace helpers::runnables
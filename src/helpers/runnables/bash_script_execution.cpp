#include "helpers/runnables/bash_script_execution.hpp"

namespace helpers::runnables
{

std::vector<std::string> BashScriptExecution::getBashArgs() const
{
    std::vector<std::string> result;

    #ifdef DEBUG
        result.push_back("-cx");
    #else
        result.push_back("-c");
    #endif

    if (this->scriptArgsStr != "")
    {
        #ifdef DEBUG
            result.push_back("bash -x " + this->scriptPath + " " + this->scriptArgsStr);
        #else
            result.push_back(this->scriptPath + " " + this->scriptArgsStr);
        #endif
    }
    else
    {
        #ifdef DEBUG
            result.push_back("bash -x " + this->scriptPath);
        #else
            result.push_back(this->scriptPath);
        #endif
    }

    return result;
}

std::map<std::string, std::string> BashScriptExecution::getBashEnvironment() const
{
    return this->scriptEnvironment;
}

BashScriptExecution::BashScriptExecution(std::string _scriptPath) :
    scriptPath(_scriptPath),
    scriptArgsStr(""),
    stdoutFilePath(""),
    stderrFilePath("")
{
    this->execution = new ExternalBinaryExecution(false, "/bin/bash", this->getBashArgs());
}

BashScriptExecution::BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr) :
    scriptPath(_scriptPath),
    scriptArgsStr(_scriptArgsStr),
    stdoutFilePath(""),
    stderrFilePath("")
{
    this->execution = new ExternalBinaryExecution(false, "/bin/bash", this->getBashArgs());
}

BashScriptExecution::BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr, std::map<std::string, std::string> _environment) :
    scriptPath(_scriptPath),
    scriptArgsStr(_scriptArgsStr),
    scriptEnvironment(_environment),
    stdoutFilePath(""),
    stderrFilePath("")
{
    this->execution = new ExternalBinaryExecution(false, "/bin/bash", this->getBashArgs(), this->getBashEnvironment());
}

BashScriptExecution::BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr, std::string _stdoutFilePath, std::string _stderrFilePath) :
    scriptPath(_scriptPath),
    scriptArgsStr(_scriptArgsStr),
    stdoutFilePath(_stdoutFilePath),
    stderrFilePath(_stderrFilePath)
{
    this->execution = new ExternalBinaryExecution(false, "/bin/bash", this->getBashArgs(), this->stdoutFilePath, this->stderrFilePath, this->getBashEnvironment());
}

BashScriptExecution::BashScriptExecution(std::string _scriptPath, std::string _scriptArgsStr, std::map<std::string, std::string> _environment, std::string _stdoutFilePath, std::string _stderrFilePath) :
    scriptPath(_scriptPath),
    scriptArgsStr(_scriptArgsStr),
    scriptEnvironment(_environment),
    stdoutFilePath(_stdoutFilePath),
    stderrFilePath(_stderrFilePath)
{
    this->execution = new ExternalBinaryExecution(false, "/bin/bash", this->getBashArgs(), this->stdoutFilePath, this->stderrFilePath);
}

void BashScriptExecution::run()
{
    this->execution->run();
}

void BashScriptExecution::wait()
{
    this->execution->wait();
}

void BashScriptExecution::end()
{
    this->execution->end();
}

std::string BashScriptExecution::getThisAsBashCommand() const
{
    return this->execution->getThisAsBashCommand();
}

BashScriptExecution::~BashScriptExecution()
{
    delete this->execution;
}

} // namespace helpers::runnables
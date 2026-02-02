#include <string>
#include <vector>

#include "external_tools_runs/cslprocess_run.hpp"
#include "helpers/runnables/external_binary_execution.hpp"
#include "helpers/string.hpp"
#include "kernel/constants.hpp"

namespace external_tools_runs
{

CSLProcessRun::CSLProcessRun(std::string _cslFilePath, int _maxSequencesDepth, std::vector<std::string> _binaryToTestArgs, std::string _outputDir) :
    cslFilePath(_cslFilePath),
    maxSequencesDepth(_maxSequencesDepth),
    binaryToTestArgs(_binaryToTestArgs),
    outputDir(_outputDir)
{
    std::vector<std::string> cslProcessArgs(4);

    cslProcessArgs[0] = this->cslFilePath;
    cslProcessArgs[1] = std::to_string(this->maxSequencesDepth);
    cslProcessArgs[2] = helpers::string::fold(this->binaryToTestArgs, ' ');
    cslProcessArgs[3] = this->outputDir;

    #ifdef DEBUG
        this->cslProcessExecution.emplace(helpers::runnables::ExternalBinaryExecution(CSLPROCESS_V2_EXECUTABLE_PATH, cslProcessArgs, "/dev/stdout", "/dev/stderr"));
    #else
        this->cslProcessExecution.emplace(helpers::runnables::ExternalBinaryExecution(CSLPROCESS_V2_EXECUTABLE_PATH, cslProcessArgs, "/dev/stdout", "/dev/null"));
    #endif
}

void CSLProcessRun::runAndWaitCSLProcess()
{
    if (this->cslProcessExecution.has_value() == true)
    {
        this->cslProcessExecution->run();
        this->cslProcessExecution->wait();
    }
}

} // namespace external_tools_runs
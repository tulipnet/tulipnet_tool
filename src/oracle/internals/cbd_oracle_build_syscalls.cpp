#include <cstddef>
#include <string>

#include "oracle/internals/cbd_oracle_build_syscalls.hpp"
#include "helpers/runnables/bash_script_execution.hpp"
#include "kernel/constants.hpp"
#include "stage/cbd_stage_interface.hpp"

namespace oracle::internals
{

void CBDOracleBuildSyscalls::jobTask() const
{
    helpers::runnables::BashScriptExecution* currentExecution;
    std::string currentArg;
    std::string outputDirForThisStage = OUTPUT_DIR_SYSCALLS "/" + this->relatedOracle.getTag();
    std::size_t currentTestingSessionIDAsStringPosition;
    std::string currentTestingSessionIDAsString;
    char* cbdPreloadEnv;
    std::map<std::string, std::string> environment;

    currentTestingSessionIDAsStringPosition = this->testingSessionResultDir.find_last_of('/');
    currentTestingSessionIDAsString = this->testingSessionResultDir.substr(currentTestingSessionIDAsStringPosition + 1);

    currentArg = this->testingSessionResultDir + " " + outputDirForThisStage + " " + this->relatedStage.getRelatedInstance().getOriginalBinaryToTestPath() + " " + this->relatedStage.getRelatedInstance().getBinaryToTestDirectory();

    cbdPreloadEnv = std::getenv(PRELOAD_ENV);

    if (cbdPreloadEnv != NULL)
    {
        environment.insert(std::pair<std::string, std::string>(PRELOAD_ENV, cbdPreloadEnv));
    }

    #ifdef DEBUG
        currentExecution = new helpers::runnables::BashScriptExecution(OUTPUT_DIR_CSL "/syscall_monitor/get_syscalls.sh", currentArg, environment);
    #else
        currentExecution = new helpers::runnables::BashScriptExecution(OUTPUT_DIR_CSL "/syscall_monitor/get_syscalls.sh", currentArg, environment, "/dev/null", "/dev/null");
    #endif

    currentExecution->run();
    currentExecution->wait();

    delete currentExecution;
}

CBDOracleBuildSyscalls::CBDOracleBuildSyscalls(const CBDOracle& _relatedOracle, const stage::CBDStageInterface& _relatedStage, std::string _testingSessionResultDir) :
    relatedOracle(_relatedOracle),
    relatedStage(_relatedStage),
    testingSessionResultDir(_testingSessionResultDir)
{
}

} // namespace oracle::internals
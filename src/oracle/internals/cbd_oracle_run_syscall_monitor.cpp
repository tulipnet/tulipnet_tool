#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "oracle/internals/cbd_oracle_run_syscall_monitor.hpp"
#include "helpers/runnables/external_binary_execution.hpp"
#include "kernel/constants.hpp"
#include "oracle/cbd_oracle.hpp"

namespace oracle::internals
{

void CBDOracleRunSyscallMonitor::jobTask() const
{
    helpers::runnables::ExternalBinaryExecution* currentSyscallMonitorRun;
    std::vector<std::string> currentSyscallMonitorRunArgs(4);
    std::string outputDirReducedSyscallsForThisStage = OUTPUT_DIR_SYSCALLS "/" + this->relatedOracle.getTag();
    std::string outputDirFullSyscallsForThisStage = OUTPUT_DIR_SYSCALLS "/" + this->relatedOracle.getTag() + "/full";
    std::string outputDirSyscallMonitorForThisStage = OUTPUT_DIR_SYSCALL_MONITOR "/" + this->relatedOracle.getTag();
    std::size_t currentTestingSessionIDAsStringPosition;
    std::string currentTestingSessionIDAsString;
    char* cbdPreloadEnv;
    std::map<std::string, std::string> environment;

    currentTestingSessionIDAsStringPosition = this->testingSessionResultDir.find_last_of('/');
    currentTestingSessionIDAsString = this->testingSessionResultDir.substr(currentTestingSessionIDAsStringPosition + 1);

    currentSyscallMonitorRunArgs[0] = this->testingSessionResultDir;
    currentSyscallMonitorRunArgs[1] = outputDirReducedSyscallsForThisStage;
    currentSyscallMonitorRunArgs[2] = outputDirFullSyscallsForThisStage;
    currentSyscallMonitorRunArgs[3] = outputDirSyscallMonitorForThisStage;

    cbdPreloadEnv = std::getenv(PRELOAD_ENV);

    if (cbdPreloadEnv != NULL)
    {
        environment.insert(std::pair<std::string, std::string>(PRELOAD_ENV, cbdPreloadEnv));
    }

    currentSyscallMonitorRun = new helpers::runnables::ExternalBinaryExecution(OUTPUT_DIR_CSL "/syscall_monitor/syscall_monitor", currentSyscallMonitorRunArgs, environment);

    currentSyscallMonitorRun->run();
    currentSyscallMonitorRun->wait();

    delete currentSyscallMonitorRun;
}

CBDOracleRunSyscallMonitor::CBDOracleRunSyscallMonitor(const CBDOracle& _relatedOracle, std::string _testingSessionResultDir) :
    relatedOracle(_relatedOracle),
    testingSessionResultDir(_testingSessionResultDir)
{
}

} // namespace oracle::internals
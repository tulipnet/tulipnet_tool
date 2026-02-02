#include <filesystem>
#include <string>
#include <vector>

#include "oracle/internals/cbd_oracle_csl_automaton_run.hpp"
#include "helpers/string.hpp"
#include "kernel/constants.hpp"

namespace oracle::internals
{

CBDOracleCSLAutomatonRun::CBDOracleCSLAutomatonRun(std::filesystem::path _syscallMonitorResultPath, std::filesystem::path _inputsDir) :
    helpers::runnables::ExternalBinaryExecution(CSLPROCESS_V2_AUTOMATON, std::vector<std::string>(), "/dev/null", "/dev/null", _syscallMonitorResultPath.string()),
    syscallMonitorResultPath(_syscallMonitorResultPath),
    inputsDir(_inputsDir)
{
}

std::filesystem::path CBDOracleCSLAutomatonRun::getSyscallMonitorResultPath() const
{
    return this->syscallMonitorResultPath;
}

std::filesystem::path CBDOracleCSLAutomatonRun::getAssociatedCommandsFilePath() const
{
    std::string syscallMonitorResultFilename;
    std::vector<std::string> splittedSyscallMonitorResultFilename;
    std::string associatedCommandsFilename;
    std::string associatedCommandsFilePath;

    syscallMonitorResultFilename = this->syscallMonitorResultPath.filename().string();
    splittedSyscallMonitorResultFilename = helpers::string::split(syscallMonitorResultFilename, '_');

    splittedSyscallMonitorResultFilename.pop_back(); // id:000091,w:2_37.adv_47,src:000065,time:3593,execs:35575,op:havoc,rep:2_26 => id:000091,w:2_37.adv_47,src:000065,time:3593,execs:35575,op:havoc,rep:2

    associatedCommandsFilename = helpers::string::fold(splittedSyscallMonitorResultFilename, '_');
    associatedCommandsFilePath = std::filesystem::path(this->inputsDir.string() + "/" + associatedCommandsFilename);

    return associatedCommandsFilePath;
}

} // namespace oracle::internals
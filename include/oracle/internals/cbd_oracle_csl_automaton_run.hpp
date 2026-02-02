#ifndef CBD_ORACLE_CSL_AUTOMATON_RUN_HPP
#define CBD_ORACLE_CSL_AUTOMATON_RUN_HPP

#include <filesystem>

namespace oracle::internals
{

class CBDOracleCSLAutomatonRun;

} // namespace oracle::internals

#include "helpers/runnables/external_binary_execution.hpp"

namespace oracle::internals
{

class CBDOracleCSLAutomatonRun : public helpers::runnables::ExternalBinaryExecution
{
    private:
        std::filesystem::path syscallMonitorResultPath;
        std::filesystem::path inputsDir;

    public:
        CBDOracleCSLAutomatonRun(std::filesystem::path _syscallMonitorResultPath, std::filesystem::path _inputsDir);

        std::filesystem::path getSyscallMonitorResultPath() const;
        std::filesystem::path getAssociatedCommandsFilePath() const;
};

} // namespace oracle::internals

#endif
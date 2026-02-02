#ifndef CBD_ORACLE_RUN_SYSCALL_MONITOR_HPP
#define CBD_ORACLE_RUN_SYSCALL_MONITOR_HPP

#include <string>

#include "oracle/cbd_oracle.hpp"
#include "helpers/runnables/job.hpp"

namespace oracle::internals
{

class CBDOracleRunSyscallMonitor : public helpers::runnables::Job
{
    private:
        const CBDOracle& relatedOracle;
        std::string testingSessionResultDir;

    protected:
        virtual void jobTask() const override;

    public:
        CBDOracleRunSyscallMonitor(const CBDOracle& _relatedOracle, std::string _testingSessionResultDir);
};

} // namespace oracle::internals

#endif
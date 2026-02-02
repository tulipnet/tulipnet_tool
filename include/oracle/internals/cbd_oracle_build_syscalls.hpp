#ifndef CBD_ORACLE_BUILD_SYSCALLS_HPP
#define CBD_ORACLE_BUILD_SYSCALLS_HPP

#include <string>

#include "helpers/runnables/job.hpp"
#include "stage/cbd_stage_interface.hpp"
#include "oracle/cbd_oracle.hpp"

namespace oracle::internals
{

class CBDOracleBuildSyscalls : public helpers::runnables::Job
{
    private:
        const CBDOracle& relatedOracle;
        const stage::CBDStageInterface& relatedStage;
        std::string testingSessionResultDir;

    protected:
        virtual void jobTask() const override;

    public:
        CBDOracleBuildSyscalls(const CBDOracle& _relatedOracle, const stage::CBDStageInterface& _relatedStage, std::string _testingSessionResultDir);
};

} // namespace cbd::internals

#endif
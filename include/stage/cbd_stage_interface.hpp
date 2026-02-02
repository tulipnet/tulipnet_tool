#ifndef CBD_STAGE_INTERFACE_HPP
#define CBD_STAGE_INTERFACE_HPP

#include <string>

namespace stage
{

class CBDStageInterface;

} // namespace stage

#include "kernel/cbd_instance.hpp"

namespace stage
{

class CBDStageInterface
{
    private:
        const kernel::CBDInstance& relatedInstance;
        int stageNumber;

    public:
        CBDStageInterface(const kernel::CBDInstance& _relatedInstance, int _stageNumber);

        virtual void runStage() = 0;
        virtual void stopStage() = 0;
        virtual std::string getGeneratedSequencesPath() const = 0;
        virtual std::string getStaticSequencesPath() const = 0;
        virtual std::string getTag() const = 0;
        virtual bool isFinishing() const = 0;

        const kernel::CBDInstance& getRelatedInstance() const;
        int getStageNumber() const;
};

} // namespace stage

#endif
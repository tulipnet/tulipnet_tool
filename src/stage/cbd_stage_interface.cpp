#include "stage/cbd_stage_interface.hpp"
#include "kernel/cbd_instance.hpp"

namespace stage
{

CBDStageInterface::CBDStageInterface(const kernel::CBDInstance& _relatedInstance, int _stageNumber) :
    relatedInstance(_relatedInstance),
    stageNumber(_stageNumber)
{
}

const kernel::CBDInstance& CBDStageInterface::getRelatedInstance() const
{
    return this->relatedInstance;
}

int CBDStageInterface::getStageNumber() const
{
    return this->stageNumber;
}

} // namespace stage
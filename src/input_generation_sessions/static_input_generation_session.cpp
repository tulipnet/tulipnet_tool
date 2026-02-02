#include <filesystem>

#include "input_generation_sessions/static_input_generation_session.hpp"

namespace input_generation_sessions
{

StaticInputGenerationSession::StaticInputGenerationSession(const stage::CBDStage* _stage, const inputs::AdversarialSequence* _adversarialSequence, std::string _resultsPath, std::string _staticSequencePath) :
    InputGenerationSession(_stage->getRelatedInstance().getOriginalBinaryToTestPath(), _stage->getRelatedInstance().getBinaryToTestArgs(), _resultsPath, _adversarialSequence->getWildcardPath(), _adversarialSequence->getID(), 1),
    adversarialSequence(_adversarialSequence),
    staticSequencePath(_staticSequencePath)
{
    std::string usedStaticFileCopyPath = this->getResultsPath() + "/command_sequence";

    this->generateTestingJob();

    std::filesystem::copy_file(this->staticSequencePath, usedStaticFileCopyPath);
}

void StaticInputGenerationSession::generateTestingJob()
{
    std::string resultsPath = this->getResultsPath();
    std::string stdoutFilePath = resultsPath + "/test_stdout";
    std::string stderrFilePath = resultsPath + "/test_stderr";

    this->testingJob = new helpers::runnables::ExternalBinaryExecution(true, this->getBinaryToTestPath(), this->getBinaryToTestArgs(), stdoutFilePath, stderrFilePath, this->staticSequencePath);
}

void StaticInputGenerationSession::run()
{
    this->setLaunched();

    this->testingJob->run();
}

void StaticInputGenerationSession::wait()
{
    this->testingJob->wait();
}

void StaticInputGenerationSession::end()
{
    this->testingJob->end();
}

bool StaticInputGenerationSession::wasAlreadyExecuted() const
{
    return this->testingJob->getWasLaunched();
}

bool StaticInputGenerationSession::isFinished() const
{
    return this->testingJob->getIsFinished();
}

StaticInputGenerationSession::~StaticInputGenerationSession()
{
    delete this->testingJob;
}

} // namespace input_generation_sessions
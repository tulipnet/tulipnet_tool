#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "stage/cbd_stage.hpp"
#include "helpers/display/terminal.hpp"
#include "helpers/runnables/bash_script_execution.hpp"
#include "input_generation_sessions/afl_run/afl_run.hpp"
#include "input_generation_sessions/fuzzing_input_generation_session.hpp"
#include "inputs/adversarial_sequences_manager.hpp"
#include "stage/cbd_stage_interface.hpp"
#include "kernel/constants.hpp"

namespace stage
{

#pragma region CBDStageUnableToCreateDirectoryException

CBDStageUnableToCreateDirectoryException::CBDStageUnableToCreateDirectoryException(std::string _directory) :
    message("ERROR : Unable to create the directory \"" + _directory + "\"\n")
{
}

const char* CBDStageUnableToCreateDirectoryException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion

void CBDStage::log(std::string message) const
{
    std::cout << helpers::display::Colors::CYAN << this->logTag << helpers::display::Colors::NEUTRAL << " " << message << std::endl;
}

void CBDStage::buildStage()
{
    mode_t directory_mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; // S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH : 0o0744
    std::string staticTestingResultsPathForThisStagePath = OUTPUT_DIR_STATIC_RESULTS "/" + std::to_string(this->getStageNumber());
    std::string currentResultPath;
    std::string wildcardsPathForTheNextStage;
    std::vector<input_generation_sessions::StaticInputGenerationSession*> currentStaticInputGenerationSessions;
    std::vector<input_generation_sessions::afl_run::AFLRun*> aflRuns;

    mkdir(staticTestingResultsPathForThisStagePath.c_str(), directory_mode);

    if ((this->getRelatedInstance().bestEffortByTimeoutModeEnabled() == false) && (this->getStageNumber() == (this->getRelatedInstance().getMaxSequencesDepth() - 1))) // We begin stage numbers by 0
    {
        this->isFinal = true;
    }
    else
    {
        this->isFinal = false;
    }

    if (this->isFinal == false)
    {
        wildcardsPathForTheNextStage = OUTPUT_DIR_NEXT_COMMAND_SEQUENCES "/" + std::to_string(this->getStageNumber() + 1);

        mkdir(wildcardsPathForTheNextStage.c_str(), directory_mode);
    }

    this->adversarialSequencesManager = new inputs::AdversarialSequencesManager(CSLPROCESS_V2_ADVERSARIAL_SEQUENCES_DIR);
    this->fuzzingInputGenerationSession = new input_generation_sessions::FuzzingInputGenerationSession(this, this->getAdversarialSequencesManager()->getWildcardedAdversarialSequences());

    if (this->isFinal == false)
    {
        this->logTag = "[CBD stage " + std::to_string(this->getStageNumber()) + "]";
    }
    else
    {
        this->logTag = "[CBD stage " + std::to_string(this->getStageNumber()) + " (final)]";
    }
}

void CBDStage::generateSHA256SumsOfGeneratedConcreteInputs() const
{
    std::filesystem::path absolutePathToCBDOutputDir = std::filesystem::absolute(OUTPUT_DIR);
    helpers::runnables::BashScriptExecution bse("/usr/local/lib/cbd/generate_sha256sums_of_generated_concrete_values.sh", absolutePathToCBDOutputDir.string());

    bse.run();
    bse.wait();
}

CBDStage::CBDStage(const kernel::CBDInstance& _relatedInstance, int _stageNumber) :
    CBDStageInterface(_relatedInstance, _stageNumber),
    launchedFuzzingRuns(false),
    finishedFuzzingRuns(false),
    reversingWildcardsOfResultsOfFinishedFuzzingRuns(false),
    reversedWildcardsOfResultsOfFinishedFuzzingRuns(false),
    launchedStaticInputGenerationSessions(false),
    finishedStaticInputGenerationSessions(false),
    builtSyscalls(false),
    ranSyscallMonitor(false),
    endedFuzzingRunsCount(0),
    previousStage(nullptr),
    timeToFuzz(_relatedInstance.getTimeToFuzzAWildcard(_stageNumber)),
    idsOfAdversarialSequencesExclusions(),
    fuzzingSessionsProgressBar(helpers::display::Colors::CYAN, this->logTag, this->timeToFuzz)
{
    this->buildStage();
}

CBDStage::CBDStage(const CBDStage& _previousStage, int _stageNumber) :
    CBDStageInterface(_previousStage.getRelatedInstance(), _stageNumber),
    launchedFuzzingRuns(false),
    finishedFuzzingRuns(false),
    reversingWildcardsOfResultsOfFinishedFuzzingRuns(false),
    reversedWildcardsOfResultsOfFinishedFuzzingRuns(false),
    launchedStaticInputGenerationSessions(false),
    finishedStaticInputGenerationSessions(false),
    builtSyscalls(false),
    ranSyscallMonitor(false),
    endedFuzzingRunsCount(0),
    previousStage(&_previousStage),
    timeToFuzz(_previousStage.getRelatedInstance().getTimeToFuzzAWildcard(_stageNumber)),
    idsOfAdversarialSequencesExclusions(),
    fuzzingSessionsProgressBar(helpers::display::Colors::CYAN, this->logTag, this->timeToFuzz)
{
    this->buildStage();
}

CBDStage::CBDStage(const CBDStage& _previousStage, int _stageNumber, std::vector<int> _idsOfAdversarialSequencesExclusions) :
    CBDStageInterface(_previousStage.getRelatedInstance(), _stageNumber),
    launchedFuzzingRuns(false),
    finishedFuzzingRuns(false),
    reversingWildcardsOfResultsOfFinishedFuzzingRuns(false),
    reversedWildcardsOfResultsOfFinishedFuzzingRuns(false),
    launchedStaticInputGenerationSessions(false),
    finishedStaticInputGenerationSessions(false),
    builtSyscalls(false),
    ranSyscallMonitor(false),
    endedFuzzingRunsCount(0),
    previousStage(&_previousStage),
    timeToFuzz(_previousStage.getRelatedInstance().getTimeToFuzzAWildcard(_stageNumber)),
    idsOfAdversarialSequencesExclusions(_idsOfAdversarialSequencesExclusions),
    fuzzingSessionsProgressBar(this->logTag, this->timeToFuzz)
{
    this->buildStage();
}

void CBDStage::runStage()
{
    this->log("Launching fuzzing and static sequences generation");

    this->fuzzingSessionsProgressBar.showAndStartIncrementingBar();

    this->fuzzingInputGenerationSession->run();
    this->fuzzingInputGenerationSession->wait();

    this->fuzzingSessionsProgressBar.joinAssociatedThread();

    this->log("Wildcard inversion");

    this->fuzzingInputGenerationSession->applyWildcardsOfResultsOfFinishedFuzzingRuns();

    if (this->isFinal == true)
    {
        this->log("Generating SHA256 sums of generated concrete files");

        this->generateSHA256SumsOfGeneratedConcreteInputs();
    }

    this->log("End of stage !");
}

void CBDStage::stopStage()
{
    if (this->finishedFuzzingRuns == false)
    {
        this->log("Stopping stage");
        this->fuzzingInputGenerationSession->run();
        this->finishedFuzzingRuns = true;
    }

    if (this->reversedWildcardsOfResultsOfFinishedFuzzingRuns == false)
    {
        this->log("Wildcard inversion");
        this->fuzzingInputGenerationSession->applyWildcardsOfResultsOfFinishedFuzzingRuns();
    }
}

std::string CBDStage::getGeneratedSequencesPath() const
{
    return OUTPUT_DIR_APPLY_WILDCARD "/" + std::to_string(this->getStageNumber());
}

std::string CBDStage::getStaticSequencesPath() const
{
    return OUTPUT_DIR_STATIC_RESULTS "/" + std::to_string(this->getStageNumber());
}

std::string CBDStage::getTag() const
{
    return "fuzz";
}

bool CBDStage::isFinishing() const
{
    return this->reversingWildcardsOfResultsOfFinishedFuzzingRuns;
}

#pragma region Getters

std::vector<input_generation_sessions::InputGenerationSession*> CBDStage::getAllInputGenerationSessions() const
{
    std::vector<input_generation_sessions::InputGenerationSession*> staticInputGenerationSessions(this->staticInputGenerationSessions.size());

    std::transform(this->staticInputGenerationSessions.begin(), this->staticInputGenerationSessions.end(), staticInputGenerationSessions.begin(),
        [](input_generation_sessions::StaticInputGenerationSession* ts)
        {
            return (input_generation_sessions::InputGenerationSession*)ts;
        }
    );

    return staticInputGenerationSessions;
}

const CBDStage* CBDStage::getPreviousStage() const
{
    return this->previousStage;
}

const inputs::AdversarialSequencesManager* CBDStage::getAdversarialSequencesManager() const
{
    return this->adversarialSequencesManager;
}

float CBDStage::getTimeToFuzz() const
{
    return this->timeToFuzz;
}

const std::vector<int> CBDStage::getIdsOfAdversarialSequencesExclusions() const
{
    return this->idsOfAdversarialSequencesExclusions;
}

#pragma endregion

CBDStage::~CBDStage()
{
    delete this->adversarialSequencesManager;
    delete this->fuzzingInputGenerationSession;
}

} // namespace stage
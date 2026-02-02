#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "external_tools_runs/external_tools_runs.hpp"
#include "input_generation_sessions/fuzzing_input_generation_session.hpp"
#include "helpers/filesystem.hpp"
#include "helpers/string.hpp"
#include "helpers/vector.hpp"
#include "input_generation_sessions/afl_run/afl_run.hpp"
#include "input_generation_sessions/afl_run/afl_run_qemu_compcov.hpp"
#include "input_generation_sessions/afl_run/afl_run_qemu_main.hpp"
#include "input_generation_sessions/afl_run/afl_run_qemu_qasan.hpp"
#include "input_generation_sessions/afl_run/afl_run_utils.hpp"
#include "input_generation_sessions/input_generation_session.hpp"
#include "inputs/generated_inputs_manager.hpp"
#include "kernel/cbd_options_manager.hpp"
#include "kernel/constants.hpp"
#include "stage/cbd_stage.hpp"

namespace input_generation_sessions
{

#pragma region NotFinishedFuzzingSessionsException

NotFinishedFuzzingSessionException::NotFinishedFuzzingSessionException()
{
}

const char* NotFinishedFuzzingSessionException::what() const throw()
{
    return "ERROR : The fuzzing session is not finished";
}

#pragma endregion

void FuzzingInputGenerationSession::prepareFuzzingSession() const
{
    std::filesystem::create_directory(this->getResultsPath());

    for (const std::filesystem::directory_entry& seedDirectory : std::filesystem::directory_iterator(OUTPUT_DIR_INITIAL_SEEDS "/0"))
    {
        std::filesystem::create_directory(this->getResultsPath() + "/" + seedDirectory.path().filename().string());
    }
}

void FuzzingInputGenerationSession::concretizeWildcards() const
{
    std::string generatedSeedPath;
    std::string currentGeneratedSeedPathForThisCore;
    std::string wildcardContent;
    std::string wildcardPrecedence;
    std::string wildcardPrecedenceContent;
    std::vector<std::string> wildcardPrecedenceAsStringVector;
    std::vector<std::string> splitWildcardContent;
    std::size_t i;
    std::vector<std::vector<std::string>> cslSeedsAtomsForThisWildcard;
    std::size_t j;
    std::ofstream seedOfstream;
    int seedId = 0;
    std::size_t k;
    std::string wildcardCoreID;

    std::filesystem::create_directory(this->initialSeedsPath);

    for (const std::filesystem::directory_entry& cpuWildcardsDirectory : std::filesystem::directory_iterator(this->generatedWildcardedAdversarialSequencePath))
    {
        currentGeneratedSeedPathForThisCore = this->initialSeedsPath + "/" + cpuWildcardsDirectory.path().filename().string();
        std::filesystem::create_directory(currentGeneratedSeedPathForThisCore);

        for (const std::filesystem::directory_entry& wildcardFile : std::filesystem::directory_iterator(cpuWildcardsDirectory))
        {
            if (std::filesystem::file_size(wildcardFile) > 0) // To prevent execution with empty files
            {
                if (this->getId() == 0)
                {
                    for (const std::filesystem::directory_entry& seedFile : std::filesystem::directory_iterator(CSLPROCESS_V2_SEEDS_DIR))
                    {
                        if (helpers::string::beginsWith(seedFile.path().filename().string(), wildcardFile.path().filename()) == true)
                        {
                            std::filesystem::copy_file(seedFile, currentGeneratedSeedPathForThisCore + "/" + seedFile.path().filename().string());
                        }
                    }
                }
                else
                {
                    // Soit le stage 1, et le wildcard ayant pour précédence 1-2-3. Il faut prendre le wildcard, et remplacer les commandes 2 et 3 par leurs seeds respectifs, trouvables dans csl/seeds_per_commands/.

                    // Example : wid=12;depth=2;content=0-3;merged1id=277 => We want to keep the part "0-3" => [0, 3] in a std::vector<int>
                    wildcardContent = helpers::filesystem::getFileContent(wildcardFile);
                    wildcardPrecedence = helpers::string::split(wildcardFile.path().filename().string(), ';').at(2);
                    wildcardPrecedenceContent = helpers::string::split(wildcardPrecedence, '=').at(1);
                    wildcardPrecedenceAsStringVector = helpers::string::split(wildcardPrecedenceContent, '-');

                    splitWildcardContent = helpers::string::split(wildcardContent, '\n');

                    wildcardCoreID = wildcardFile.path().parent_path().filename().string();

                    if (wildcardPrecedence.size() >= (std::size_t)this->getId())
                    {
                        j = 0;

                        for (i = this->getId(); i < wildcardPrecedenceAsStringVector.size(); ++i)
                        {
                            cslSeedsAtomsForThisWildcard.push_back(std::vector<std::string>());

                            for (const std::filesystem::directory_entry& cslSeedsPerCommandsIterator : std::filesystem::directory_iterator(CSLPROCESS_V2_SEEDS_PER_COMMANDS_DIR))
                            {
                                if (helpers::string::beginsWith(cslSeedsPerCommandsIterator.path().filename().string(), "cmd_id:" + wildcardPrecedenceAsStringVector.at(i)) == true)
                                {
                                    cslSeedsAtomsForThisWildcard.at(j).push_back(
                                        helpers::filesystem::getFileContent(cslSeedsPerCommandsIterator.path())
                                    );
                                }
                            }

                            ++j;
                        }
                    }

                    std::filesystem::create_directories(OUTPUT_DIR_INITIAL_SEEDS "/" + std::to_string(this->getId()) + "/" + wildcardCoreID);

                    for (std::vector<std::string>& currentVector : cslSeedsAtomsForThisWildcard)
                    {
                        if (currentVector.size() > 0)
                        {
                            for (std::string& seed : currentVector)
                            {
                                seedOfstream = std::ofstream(OUTPUT_DIR_INITIAL_SEEDS "/" + std::to_string(this->getId()) + "/" + wildcardCoreID + "/" + wildcardFile.path().filename().string() + ";seedid=" + std::to_string(seedId));
                                k = 0;

                                while ((k < (std::size_t)this->getId()) && (k < splitWildcardContent.size()))
                                {
                                    seedOfstream << splitWildcardContent.at(k) << "\n";

                                    ++k;
                                }

                                seedOfstream << seed << "\n" << std::endl;

                                ++seedId;
                            }
                        }
                    }

                    cslSeedsAtomsForThisWildcard.clear();
                }
            }
        }
    }
}

void FuzzingInputGenerationSession::generateAFLRuns()
{
    bool isAnExploitAFLRun = false;

    if ((this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::ITERATIVE_MODE_STRATEGIES_BALANCING) == true) && (this->relatedStage->getRelatedInstance().getIterationNumber() % 2 == 1))
    {
        isAnExploitAFLRun = true;
    }

    if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY) == true)
    {
        for (const std::filesystem::directory_entry& outputResultDir : std::filesystem::directory_iterator(this->getResultsPath()))
        {
            this->aflRuns.push_back(
                new afl_run::AFLRunQemuMain(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getOriginalBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            this->aflRuns.push_back(
                new afl_run::AFLRunQemuCompcov(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getOriginalBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            this->aflRuns.push_back(
                new afl_run::AFLRunQemuQasan(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getOriginalBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );
        }
    }
    else if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_CMPLOG_ONLY) == true)
    {
        for (const std::filesystem::directory_entry& outputResultDir : std::filesystem::directory_iterator(this->getResultsPath()))
        {
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    true,
                    "main",
                    this->relatedStage->getRelatedInstance().getCmplogBinaryToTestPath(),
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );
        }
    }
    else if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_COMPCOV_ONLY) == true)
    {
        for (const std::filesystem::directory_entry& outputResultDir : std::filesystem::directory_iterator(this->getResultsPath()))
        {
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    true,
                    "main",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getCompcovBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );
        }
    }
    else if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_STANDARD_ONLY) == true)
    {
        for (const std::filesystem::directory_entry& outputResultDir : std::filesystem::directory_iterator(this->getResultsPath()))
        {
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    true,
                    "main",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );
        }
    }
    else if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_DEGRADED) == true)
    {
        for (const std::filesystem::directory_entry& outputResultDir : std::filesystem::directory_iterator(this->getResultsPath()))
        {
            // Main instance
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    true,
                    "main",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            // Cmplog instance
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    false,
                    "sec-cmplog",
                    this->relatedStage->getRelatedInstance().getCmplogBinaryToTestPath(),
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            // CompCov instance
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    false,
                    "sec-compcov",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getCompcovBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );
        }
    }
    else if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_FULL) == true)
    {
        for (const std::filesystem::directory_entry& outputResultDir : std::filesystem::directory_iterator(this->getResultsPath()))
        {
            // Main instance
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    true,
                    "main",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            // Cmplog instance
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    false,
                    "sec-cmplog",
                    this->relatedStage->getRelatedInstance().getCmplogBinaryToTestPath(),
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            // CompCov instances
            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    false,
                    "sec-compcov1",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getCompcovBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    false,
                    "sec-compcov2",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getCompcovBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );

            this->aflRuns.push_back(
                new afl_run::AFLRun(
                    this->relatedStage->getTimeToFuzz(),
                    false,
                    this->initialSeedsPath + "/" + outputResultDir.path().filename().string(),
                    outputResultDir.path().string(),
                    false,
                    "sec-compcov3",
                    this->generatedWildcardedAdversarialSequencePath + "/" + outputResultDir.path().filename().string(),
                    this->relatedStage->getRelatedInstance().getCompcovBinaryToTestPath(),
                    this->relatedStage->getRelatedInstance().getBinaryToTestArgs(),
                    isAnExploitAFLRun
                )
            );
        }
    }
}

void FuzzingInputGenerationSession::forAllAFLRuns(void (*action)(const afl_run::AFLRun* _aflRun)) const
{
    for (const afl_run::AFLRun* aflRun : this->aflRuns)
    {
        action(aflRun);
    }
}

void FuzzingInputGenerationSession::generateWildcardsForThisSession()
{
    int id = this->getId();
    std::string currentWildcardedAdversarialSequenceName;
    std::string currentGeneratedWildcardedAdversarialSequencePath;
    int i = 0;
    std::vector<std::filesystem::path> adversarialSequencesPathsForThisCPUCore;
    std::string wildcardsForThisCPUCorePath;
    std::vector<std::filesystem::path> wildcardsPath;
    std::vector<std::string> allWildcardedAdversarialSequencesFilenames;
    std::vector<std::filesystem::path> allWildcardedAdversarialSequencesPaths;
    std::vector<std::vector<std::string>> dispatchedWildcardsFilenamesByCPUs;
    bool match;
    int numberOfConcurrentSessions;

    std::filesystem::create_directory(this->generatedWildcardedAdversarialSequencePath);

    if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY))
    {
        numberOfConcurrentSessions = this->getCPUCost() / 3;
    }
    else if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_DEGRADED))
    {
        numberOfConcurrentSessions = this->getCPUCost() / 3;
    }
    else if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::FUZZING_MODE_FULL))
    {
        numberOfConcurrentSessions = this->getCPUCost() / 5;
    }
    else
    {
        numberOfConcurrentSessions = this->getCPUCost();
    }

    if (id == 0)
    {
        allWildcardedAdversarialSequencesFilenames = this->generatedInputsManager->getAllWildcardedAdversarialSequencesFilenames();

        for (const std::string& adversarialSequenceFilename : allWildcardedAdversarialSequencesFilenames)
        {
            allWildcardedAdversarialSequencesPaths.push_back(CSLPROCESS_V2_ADVERSARIAL_SEQUENCES_DIR "/" + adversarialSequenceFilename);
        }

        for (i = 0; i < numberOfConcurrentSessions; ++i)
        {
            adversarialSequencesPathsForThisCPUCore = helpers::vector::getChunk(allWildcardedAdversarialSequencesPaths, numberOfConcurrentSessions, i);
            wildcardsForThisCPUCorePath = this->generatedWildcardedAdversarialSequencePath + "/coreid=" + std::to_string(i);

            std::filesystem::create_directory(wildcardsForThisCPUCorePath);

            for (const std::filesystem::path& wildcardedAdversarialSequencePath : adversarialSequencesPathsForThisCPUCore)
            {
                currentWildcardedAdversarialSequenceName = wildcardedAdversarialSequencePath.filename().string();
                currentGeneratedWildcardedAdversarialSequencePath = wildcardsForThisCPUCorePath + "/" + currentWildcardedAdversarialSequenceName;

                external_tools_runs::runPutConcreteDataOn2ndAndNextWildcards(wildcardedAdversarialSequencePath.string(), DEFAULT_CONCRETE_VALUE, currentGeneratedWildcardedAdversarialSequencePath);
            }
        }
    }
    else
    {
        this->generatedInputsManager->generateWildcardFilesForThisDepth();

        dispatchedWildcardsFilenamesByCPUs = generatedInputsManager->dispatchWildcardsFilenamesOnNCPUs(id, numberOfConcurrentSessions);

        wildcardsPath = helpers::filesystem::getDirectoryContent(this->generatedWildcardedAdversarialSequencePath);

        for (const std::vector<std::string>& wildcardsForThisCPU : dispatchedWildcardsFilenamesByCPUs)
        {
            wildcardsForThisCPUCorePath = this->generatedWildcardedAdversarialSequencePath + "/coreid=" + std::to_string(i);

            std::filesystem::create_directory(wildcardsForThisCPUCorePath);

            for (const std::filesystem::path& adversarialSequencePath : std::filesystem::directory_iterator(OUTPUT_DIR_GENERATED_WILDCARDS "/" + std::to_string(id)))
            {
                std::function<bool(const std::string&)> function = [adversarialSequencePath](const std::string& wildcardFilename) -> bool
                {
                    std::string adversarialSequenceFilename = adversarialSequencePath.filename().string();

                    return helpers::string::beginsWith(adversarialSequenceFilename, wildcardFilename);
                };

                match = helpers::vector::match(wildcardsForThisCPU, function);

                if (match == true)
                {
                    std::filesystem::rename(adversarialSequencePath, std::filesystem::path(wildcardsForThisCPUCorePath + "/" + adversarialSequencePath.filename().string()));
                }
            }

            ++i;
        }
    }
}

void FuzzingInputGenerationSession::generateStaticSequencesForThisSession()
{
    int id = this->getId();
    std::filesystem::path currentStageAdversarialSequencesPath;
    std::filesystem::path currentStaticAdversarialSequencePath;
    std::string currentWildcardFileContent;

    currentStageAdversarialSequencesPath = OUTPUT_DIR_STATIC_RESULTS "/" + std::to_string(id);

    for (const std::string& staticAdversarialSequenceFilename : this->generatedInputsManager->getAllStaticAdversarialSequencesFilenames())
    {
        currentStaticAdversarialSequencePath = std::filesystem::path(CSLPROCESS_V2_ADVERSARIAL_SEQUENCES_DIR "/" + staticAdversarialSequenceFilename);

        std::filesystem::copy_file(currentStaticAdversarialSequencePath, std::filesystem::path(currentStageAdversarialSequencesPath.string() + "/" + currentStaticAdversarialSequencePath.filename().string()));
    }

    if (id > 0)
    {
        for (const std::filesystem::directory_entry& coreDirectory : std::filesystem::directory_iterator(OUTPUT_DIR_GENERATED_WILDCARDS "/" + std::to_string(id)))
        {
            for (const std::filesystem::directory_entry& generatedWildcardFile : std::filesystem::directory_iterator(coreDirectory))
            {
                currentWildcardFileContent = helpers::filesystem::getFileContent(generatedWildcardFile.path());

                if (helpers::string::countOccurrencesOfCharacter(currentWildcardFileContent, '*') == 0)
                {
                    std::filesystem::rename(generatedWildcardFile.path(), OUTPUT_DIR_STATIC_RESULTS "/" + std::to_string(id) + "/" + generatedWildcardFile.path().filename().string());
                }
            }
        }
    }
}

FuzzingInputGenerationSession::FuzzingInputGenerationSession(const stage::CBDStage* _relatedStage, std::vector<std::filesystem::path> _adversarialSequences) :
    InputGenerationSession(_relatedStage->getRelatedInstance().getBinaryToTestPath(),
        _relatedStage->getRelatedInstance().getBinaryToTestArgs(),
        OUTPUT_DIR_FUZZING_RESULTS "/" + std::to_string(_relatedStage->getStageNumber()),
        OUTPUT_DIR_FUZZING_RESULTS "/" + std::to_string(_relatedStage->getStageNumber()) + "/main/queue",
        _relatedStage->getStageNumber(),
        _relatedStage->getRelatedInstance().getNumberOfConcurrentThreads()
    ),
    relatedStage(_relatedStage),
    initialSeedsPath(OUTPUT_DIR_INITIAL_SEEDS "/" + std::to_string(_relatedStage->getStageNumber())),
    finished(false),
    adversarialSequences(_adversarialSequences),
    generatedWildcardedAdversarialSequencePath(OUTPUT_DIR_GENERATED_WILDCARDS "/" + std::to_string(_relatedStage->getStageNumber()))
{
    int id = this->getId();

    if (id == 0)
    {
        if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::ITERATIVE_MODE) == true)
        {
            this->generatedInputsManager = inputs::GeneratedInputsManager(
                "iteration_" + std::to_string(this->relatedStage->getRelatedInstance().getIterationNumber())
            );
        }
        else
        {
            this->generatedInputsManager = inputs::GeneratedInputsManager();
        }
    }
    else
    {
        if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::ITERATIVE_MODE) == true)
        {
            this->generatedInputsManager = inputs::GeneratedInputsManager(
                "iteration_" + std::to_string(this->relatedStage->getRelatedInstance().getIterationNumber()),
                OUTPUT_DIR_APPLY_WILDCARD "/" + std::to_string(id - 1),
                id
            );
        }
        else
        {
            this->generatedInputsManager = inputs::GeneratedInputsManager(OUTPUT_DIR_APPLY_WILDCARD "/" + std::to_string(id - 1), id);
        }
    }

    this->generateWildcardsForThisSession();
    this->generateStaticSequencesForThisSession();

    if (this->relatedStage->getRelatedInstance().getOptionsManager().isDefined(kernel::CBDOptionsManager::Options::ITERATIVE_MODE) == true)
    {
        std::filesystem::copy(
            this->relatedStage->getRelatedInstance().getInitialSeedsPath() + "/" + std::to_string(_relatedStage->getStageNumber()),
            this->initialSeedsPath,
            std::filesystem::copy_options::recursive
        );

        std::filesystem::copy(
            this->relatedStage->getRelatedInstance().getGotWildcardsPath() + "/" + std::to_string(_relatedStage->getStageNumber()),
            this->generatedWildcardedAdversarialSequencePath,
            std::filesystem::copy_options::recursive
        );
    }
    else
    {
        this->concretizeWildcards();
    }

    this->prepareFuzzingSession();
    this->generateAFLRuns();
}

void FuzzingInputGenerationSession::run()
{
    this->setLaunched();

    this->forAllAFLRuns([](const afl_run::AFLRun* aflRun)
    {
        aflRun->run();
    });
}

void FuzzingInputGenerationSession::end()
{
    this->forAllAFLRuns([](const afl_run::AFLRun* aflRun)
    {
        aflRun->end();
    });
}

void FuzzingInputGenerationSession::wait()
{
    this->forAllAFLRuns([](const afl_run::AFLRun* aflRun)
    {
        aflRun->wait();
    });

    this->finished = true;
}

bool FuzzingInputGenerationSession::wasAlreadyExecuted() const
{
    return this->wasLaunched();
}

bool FuzzingInputGenerationSession::isFinished() const
{
    return this->finished;
}

bool FuzzingInputGenerationSession::applyWildcardsOfResultsOfFinishedFuzzingRuns() const
{
    std::string reverseWildcardsPath = OUTPUT_DIR_APPLY_WILDCARD "/" + std::to_string(this->relatedStage->getStageNumber());
    std::filesystem::path currentGeneratedInputPath;
    std::string wildcardParameterTagInGeneratedInputFilename = afl_run::utils::getAFLQueueFilenameParameterKey(afl_run::utils::AFLQueueFilenameParameter::WILDCARD);
    std::map<std::string, std::string> currentGeneratedInputParameters;
    std::string currentGeneratedInputAssociatedWildcard;
    std::string currentWildcardPath;
    std::string reversedWildcardPath;
    std::string currentCoreIDFilename;
    std::filesystem::path generatedInputsPath;

    if (this->isFinished() == true)
    {
        std::filesystem::create_directory(reverseWildcardsPath);

        for (const std::filesystem::directory_entry& generatedInputsDirForThisCore : std::filesystem::directory_iterator(OUTPUT_DIR_FUZZING_RESULTS "/" + std::to_string(this->getId())))
        {
            currentCoreIDFilename = generatedInputsDirForThisCore.path().filename().string();

            generatedInputsPath = generatedInputsDirForThisCore.path().string() + "/main/queue";

            if (std::filesystem::exists(generatedInputsPath) == true)
            {
                for (const std::filesystem::directory_entry& generatedInputPath : std::filesystem::directory_iterator(generatedInputsPath))
                {
                    currentGeneratedInputPath = generatedInputPath.path();
                    currentGeneratedInputParameters = afl_run::utils::getParametersFromAFLQueueFilename(currentGeneratedInputPath.filename().string());

                    if (currentGeneratedInputParameters.count(wildcardParameterTagInGeneratedInputFilename) > 0)
                    {
                        currentGeneratedInputAssociatedWildcard = currentGeneratedInputParameters.at(wildcardParameterTagInGeneratedInputFilename);
                        reversedWildcardPath = reverseWildcardsPath + "/" + generatedInputPath.path().filename().string();

                        currentWildcardPath = this->generatedWildcardedAdversarialSequencePath + "/" + currentCoreIDFilename + "/" + currentGeneratedInputAssociatedWildcard;

                        external_tools_runs::runApplyWildcard(currentWildcardPath, generatedInputPath.path().string(), reversedWildcardPath);
                    }
                }
            }

            generatedInputsPath = generatedInputsDirForThisCore.path().string() + "/main/hangs";

            for (const afl_run::AFLRun* aflRun : this->aflRuns)
            {
                generatedInputsPath = generatedInputsDirForThisCore.path().string() + "/" + aflRun->getAFLInstanceName() + "/hangs";

                if (std::filesystem::exists(generatedInputsPath) == true)
                {
                    for (const std::filesystem::directory_entry& generatedInputPath : std::filesystem::directory_iterator(generatedInputsPath))
                    {
                        if (generatedInputsPath.filename().string() != "README.md")
                        {
                            currentGeneratedInputPath = generatedInputPath.path();
                            currentGeneratedInputParameters = afl_run::utils::getParametersFromAFLQueueFilename(currentGeneratedInputPath.filename().string());

                            if (currentGeneratedInputParameters.count(wildcardParameterTagInGeneratedInputFilename) > 0)
                            {
                                currentGeneratedInputAssociatedWildcard = currentGeneratedInputParameters.at(wildcardParameterTagInGeneratedInputFilename);
                                reversedWildcardPath = reverseWildcardsPath + "/" + generatedInputPath.path().filename().string() + "_hang_" + aflRun->getAFLInstanceName();

                                // TODO : Ici à remplir
                                /*
                                - Récupérer le chemin entier du wildcard (5.cmd -> ./out/wildcards/5.cmd (Exemple))
                                - Reverse le wildcard dans le bon dossier (S'assurer de sa création au préalable)
                                */
                                currentWildcardPath = this->generatedWildcardedAdversarialSequencePath + "/" + currentCoreIDFilename + "/" + currentGeneratedInputAssociatedWildcard;

                                external_tools_runs::runApplyWildcard(currentWildcardPath, generatedInputPath.path().string(), reversedWildcardPath);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        throw new NotFinishedFuzzingSessionException();
    }

    return false;
}

FuzzingInputGenerationSession::~FuzzingInputGenerationSession()
{
    for (const afl_run::AFLRun* aflRun : this->aflRuns)
    {
        delete aflRun;
    }
}

} // namespace input_generation_sessions
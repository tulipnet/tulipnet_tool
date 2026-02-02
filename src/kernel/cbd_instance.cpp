#include <optional>
#include <string>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>

#include "kernel/cbd_instance.hpp"
#include "external_tools_runs/cslprocess_run.hpp"
#include "helpers/time/timer.hpp"
#include "kernel/cbd_options_manager.hpp"
#include "kernel/constants.hpp"
#include "helpers/runnables/bash_script_execution.hpp"

namespace kernel
{

#pragma region CBDInstanceUninitializedException

CBDInstanceUninitializedException::CBDInstanceUninitializedException()
{
}

const char* CBDInstanceUninitializedException::what() const throw()
{
    return "ERROR : Uninitialized CBDInstance\n";
}

#pragma endregion
#pragma region CBDInstanceRelatedToolNotFoundException

CBDInstanceRelatedToolNotFoundException::CBDInstanceRelatedToolNotFoundException(std::string _relatedTool)
{
    message = ("ERROR : The related tool " + _relatedTool + " was not found. Maybe you have to compile it or check if it is executable for its owner.\n");
}

CBDInstanceRelatedToolNotFoundException::CBDInstanceRelatedToolNotFoundException(std::string _relatedTool, std::string _reason)
{
    message = ("ERROR : The related tool " + _relatedTool + " cannot be run. Reason : " + _reason + "\n");
}

const char* CBDInstanceRelatedToolNotFoundException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion
#pragma region CBDInstanceOutputDirectoryAlreadyCreatedException

CBDInstanceOutputDirectoryAlreadyCreatedException::CBDInstanceOutputDirectoryAlreadyCreatedException()
{
}

const char* CBDInstanceOutputDirectoryAlreadyCreatedException::what() const throw()
{
    return "ERROR : The directory \"" OUTPUT_DIR "\" already exists.\n";
}

#pragma endregion
#pragma region CBDInstance

CBDInstance* CBDInstance::instance = nullptr;

void CBDInstance::checkRelatedTools(std::string toolName, std::string toolPath)
{
    struct stat currentStat;
    int currentResult;
    mode_t rxForOwner = S_IRUSR | S_IXUSR; // 0o0500

    // Check if the tool is compiled
    currentResult = stat(toolPath.c_str(), &currentStat);

    if (currentResult == 0)
    {
        // Check if the tool is executable for its owner
        if ((currentStat.st_mode & rxForOwner) != rxForOwner)
        {
            throw CBDInstanceRelatedToolNotFoundException(toolName, "Not readable and/or executable for its owner");
        }
    }
    else
    {
        throw CBDInstanceRelatedToolNotFoundException(toolName);
    }
}

CBDInstance::CBDInstance(std::string _cslFilePath, std::string _binaryToTestPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToTestArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager) :
    cslFilePath(_cslFilePath),
    binaryToTestPathRadical(_binaryToTestPathRadical),
    timeToFuzzAWildcard(_timeToFuzzAWildcard),
    maxSequencesDepth(_maxSequencesDepth),
    initialFuzzingSeedsPath(_initialFuzzingSeedsPath),
    binaryToTestArgs(_binaryToTestArgs),
    numberOfConcurrentThreads(_numberOfConcurrentThreads),
    optionsManager(_optionsManager),
    initialSeedsPath(std::nullopt),
    gotWildcardsPath(std::nullopt),
    iterationNumber(std::nullopt),
    iterativeModeStrategiesBalancingMode(std::nullopt)
{
    mode_t directory_mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; // S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH : 0o0744
    std::optional<external_tools_runs::CSLProcessRun> cslprocessRun;
    int mkdirResult;
    helpers::runnables::BashScriptExecution removeTrivialSequences = helpers::runnables::BashScriptExecution("/usr/local/lib/cbd/delete_trivial_adversarial_sequences.sh", OUTPUT_DIR_CSL "/adversarial_sequences/");
    char* cslProcessStuffEnvContent;

    // Checks
    CBDInstance::checkRelatedTools("cslprocess", CSLPROCESS_V2_EXECUTABLE_PATH);
    CBDInstance::checkRelatedTools("apply_wildcard", APPLY_WILDCARD_EXECUTABLE_PATH);
    CBDInstance::checkRelatedTools("put_concrete_data_on_2nd_and_next_wildcards", PUT_CONCRETE_DATA_ON_2ND_AND_NEXT_WILDCARDS_EXECUTABLE_PATH);
    CBDInstance::checkRelatedTools("concretize_wildcard", CONCRETIZE_WILDCARD_EXECUTABLE_PATH);

    // Directory creation
    mkdirResult = mkdir(OUTPUT_DIR, directory_mode);

    if (mkdirResult != -1)
    {
        cslProcessStuffEnvContent = std::getenv(CSLPROCESS_STUFF_ENV);

        if (cslProcessStuffEnvContent == NULL)
        {
            cslprocessRun = external_tools_runs::CSLProcessRun(this->cslFilePath, this->getMaxSequencesDepth(), this->getBinaryToTestArgs(), OUTPUT_DIR_CSL);

            mkdir(OUTPUT_DIR_CSL, directory_mode);

            // csl_process execution
            cslprocessRun->runAndWaitCSLProcess();
        }
        else
        {
            //std::filesystem::copy(cslProcessStuffEnvContent, OUTPUT_DIR_CSL, std::filesystem::copy_options::recursive);
            symlink(cslProcessStuffEnvContent, OUTPUT_DIR_CSL);
        }

        mkdir(OUTPUT_DIR_FUZZING_RESULTS, directory_mode);
        mkdir(OUTPUT_DIR_STATIC_RESULTS, directory_mode);

        if (this->optionsManager->isDefined(CBDOptionsManager::Options::DISCARD_TRIVIAL_ADVERSARIAL_SEQUENCES) == true)
        {
            removeTrivialSequences.run();
            removeTrivialSequences.wait();
        }

        // Mkdirs for other steps
        mkdir(OUTPUT_DIR_APPLY_WILDCARD, directory_mode);
        mkdir(OUTPUT_DIR_SYSCALLS, directory_mode);
        mkdir(OUTPUT_DIR_SYSCALL_MONITOR, directory_mode);
        mkdir(OUTPUT_DIR_NEXT_COMMAND_SEQUENCES, directory_mode);
        mkdir(OUTPUT_DIR_POTENTIAL_BACKDOORS, directory_mode);
        mkdir(OUTPUT_DIR_INITIAL_SEEDS, directory_mode);
        mkdir(OUTPUT_DIR_GENERATED_WILDCARDS, directory_mode);

        // Generation of the AllCommandManager
        this->allCommandsManager = new inputs::AllCommandsManager(CSLPROCESS_V2_ALL_COMMANDS_DIR);
    }
    else
    {
        throw CBDInstanceOutputDirectoryAlreadyCreatedException();
    }

    this->bestEffortByTimeoutMode = this->optionsManager->isDefined(CBDOptionsManager::Options::BEST_EFFORT_BY_TIMEOUT);
}

CBDInstance::CBDInstance(std::string _cslFilePath, std::string _binaryToTestPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToTestArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager, std::string _initialSeedsPath, std::string _gotWildcardsPath, int _iterationNumber,  bool _iterativeModeStrategiesBalancingMode) :
    cslFilePath(_cslFilePath),
    binaryToTestPathRadical(_binaryToTestPathRadical),
    timeToFuzzAWildcard(_timeToFuzzAWildcard),
    maxSequencesDepth(_maxSequencesDepth),
    initialFuzzingSeedsPath(_initialFuzzingSeedsPath),
    binaryToTestArgs(_binaryToTestArgs),
    numberOfConcurrentThreads(_numberOfConcurrentThreads),
    optionsManager(_optionsManager),
    initialSeedsPath(_initialSeedsPath),
    gotWildcardsPath(_gotWildcardsPath),
    iterationNumber(_iterationNumber),
    iterativeModeStrategiesBalancingMode(_iterativeModeStrategiesBalancingMode)
{
    mode_t directory_mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; // S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH : 0o0744
    std::optional<external_tools_runs::CSLProcessRun> cslprocessRun;
    int mkdirResult;
    helpers::runnables::BashScriptExecution removeTrivialSequences = helpers::runnables::BashScriptExecution("/usr/local/lib/cbd/delete_trivial_adversarial_sequences.sh", OUTPUT_DIR_CSL "/adversarial_sequences/");
    char* cslProcessStuffEnvContent;

    // Checks
    CBDInstance::checkRelatedTools("cslprocess", CSLPROCESS_V2_EXECUTABLE_PATH);
    CBDInstance::checkRelatedTools("apply_wildcard", APPLY_WILDCARD_EXECUTABLE_PATH);
    CBDInstance::checkRelatedTools("put_concrete_data_on_2nd_and_next_wildcards", PUT_CONCRETE_DATA_ON_2ND_AND_NEXT_WILDCARDS_EXECUTABLE_PATH);
    CBDInstance::checkRelatedTools("concretize_wildcard", CONCRETIZE_WILDCARD_EXECUTABLE_PATH);

    // Directory creation
    mkdirResult = mkdir(OUTPUT_DIR, directory_mode);

    if (mkdirResult != -1)
    {
        cslProcessStuffEnvContent = std::getenv(CSLPROCESS_STUFF_ENV);

        if (cslProcessStuffEnvContent == NULL)
        {
            cslprocessRun = external_tools_runs::CSLProcessRun(this->cslFilePath, this->getMaxSequencesDepth(), this->getBinaryToTestArgs(), OUTPUT_DIR_CSL);

            mkdir(OUTPUT_DIR_CSL, directory_mode);

            // csl_process execution
            cslprocessRun->runAndWaitCSLProcess();
        }
        else
        {
            //std::filesystem::copy(cslProcessStuffEnvContent, OUTPUT_DIR_CSL, std::filesystem::copy_options::recursive);
            symlink(cslProcessStuffEnvContent, OUTPUT_DIR_CSL);
        }

        mkdir(OUTPUT_DIR_FUZZING_RESULTS, directory_mode);
        mkdir(OUTPUT_DIR_STATIC_RESULTS, directory_mode);

        if (this->optionsManager->isDefined(CBDOptionsManager::Options::DISCARD_TRIVIAL_ADVERSARIAL_SEQUENCES) == true)
        {
            removeTrivialSequences.run();
            removeTrivialSequences.wait();
        }

        // Mkdirs for other steps
        mkdir(OUTPUT_DIR_APPLY_WILDCARD, directory_mode);
        mkdir(OUTPUT_DIR_SYSCALLS, directory_mode);
        mkdir(OUTPUT_DIR_SYSCALL_MONITOR, directory_mode);
        mkdir(OUTPUT_DIR_NEXT_COMMAND_SEQUENCES, directory_mode);
        mkdir(OUTPUT_DIR_POTENTIAL_BACKDOORS, directory_mode);
        mkdir(OUTPUT_DIR_INITIAL_SEEDS, directory_mode);
        mkdir(OUTPUT_DIR_GENERATED_WILDCARDS, directory_mode);

        // Generation of the AllCommandManager
        this->allCommandsManager = new inputs::AllCommandsManager(CSLPROCESS_V2_ALL_COMMANDS_DIR);
    }
    else
    {
        throw CBDInstanceOutputDirectoryAlreadyCreatedException();
    }

    this->bestEffortByTimeoutMode = this->optionsManager->isDefined(CBDOptionsManager::Options::BEST_EFFORT_BY_TIMEOUT);
}

CBDInstance& CBDInstance::initializeInstance(std::string _cslFilePath, std::string _binaryToFuzzPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToFuzzArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager)
{
    if (instance != nullptr)
    {
        delete instance;
    }

    instance = new CBDInstance(_cslFilePath, _binaryToFuzzPathRadical, _timeToFuzzAWildcard, _maxSequencesDepth, _initialFuzzingSeedsPath, _binaryToFuzzArgs, _numberOfConcurrentThreads, _optionsManager);

    return *instance;
}

CBDInstance& CBDInstance::initializeInstance(std::string _cslFilePath, std::string _binaryToFuzzPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToFuzzArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager, std::string _initialSeedsPath, std::string _gotWildcardsPath, int _iterationNumber,  bool _iterativeModeStrategiesBalancingMode)
{
    if (instance != nullptr)
    {
        delete instance;
    }

    instance = new CBDInstance(_cslFilePath, _binaryToFuzzPathRadical, _timeToFuzzAWildcard, _maxSequencesDepth, _initialFuzzingSeedsPath, _binaryToFuzzArgs, _numberOfConcurrentThreads, _optionsManager, _initialSeedsPath, _gotWildcardsPath, _iterationNumber, _iterativeModeStrategiesBalancingMode);

    return *instance;
}

CBDInstance& CBDInstance::getInstance()
{
    if (instance == nullptr)
    {
        throw CBDInstanceUninitializedException();
    }

    return *instance;
}

std::string CBDInstance::getOriginalBinaryToTestPath() const
{
    return this->binaryToTestPathRadical + ".ori";
}

std::string CBDInstance::getAFLOptimizedBinaryToTestPath() const
{
    return this->binaryToTestPathRadical + ".lto.afl";
}

std::string CBDInstance::getCmplogBinaryToTestPath() const
{
    return this->binaryToTestPathRadical + ".lto.cmplog";
}

std::string CBDInstance::getCompcovBinaryToTestPath() const
{
    return this->binaryToTestPathRadical + ".laf-intel.afl";
}

void CBDInstance::enableAFLSystemOptimizations() const
{
    helpers::runnables::BashScriptExecution bse(AFL_FUZZ_SYSTEM_OPTIMIZATION_SCRIPT_PATH, "", "/dev/null", "/dev/null");

    bse.run();
    bse.wait();
}

std::string CBDInstance::getBinaryToTestDirectory() const
{
    std::string binaryToTestPath = this->getOriginalBinaryToTestPath();
    std::filesystem::path binaryToTestPathAsPath = std::filesystem::path(binaryToTestPath);
    std::filesystem::path absoluteBinaryToTestPathAsPath = std::filesystem::absolute(binaryToTestPathAsPath);
    std::filesystem::path canonicalAbsoluteBinaryToTestPathAsPath = std::filesystem::canonical(absoluteBinaryToTestPathAsPath);

    return canonicalAbsoluteBinaryToTestPathAsPath.parent_path().string();
}

std::string CBDInstance::getBinaryToTestPath() const
{
    std::string result;

    if (this->optionsManager->isDefined(CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY) == true)
    {
        result = this->getOriginalBinaryToTestPath();
    }
    else
    {
        result = this->getAFLOptimizedBinaryToTestPath();
    }

    return result;
}

void CBDInstance::increaseMaxSequenceDepthAndRerunCSLProcess()
{
    std::optional<external_tools_runs::CSLProcessRun> cslProcessRun;

    // Cleaning and directory regeneration
    std::filesystem::remove_all(OUTPUT_DIR_CSL);
    std::filesystem::create_directory(OUTPUT_DIR_CSL);

    this->maxSequencesDepth++;

    cslProcessRun = external_tools_runs::CSLProcessRun(this->cslFilePath, this->getMaxSequencesDepth(), this->getBinaryToTestArgs(), OUTPUT_DIR_CSL);

    cslProcessRun->runAndWaitCSLProcess();
}

float CBDInstance::getTimeToFuzzAWildcard(int stageNumber) const
{
    float result;
    int maxStageNumber = this->getMaxSequencesDepth() - 1;

    if ((stageNumber >= maxStageNumber) && (this->bestEffortByTimeoutModeEnabled() == true))
    {
        return this->timeToFuzzAWildcard.at(this->timeToFuzzAWildcard.size() - 1);
    }
    else
    {
        return this->timeToFuzzAWildcard.at(stageNumber);
    }

    return result;
}

#pragma region Getters

const std::vector<float>& CBDInstance::getTimeToFuzzAWildcard() const
{
    return this->timeToFuzzAWildcard;
}

std::string CBDInstance::getInitialFuzzingSeedsPath() const
{
    return this->initialFuzzingSeedsPath;
}

std::vector<std::string> CBDInstance::getBinaryToTestArgs() const
{
    return this->binaryToTestArgs;
}

int CBDInstance::getMaxSequencesDepth() const
{
    return this->maxSequencesDepth;
}

int CBDInstance::getNumberOfConcurrentThreads() const
{
    return this->numberOfConcurrentThreads;
}

const inputs::AllCommandsManager& CBDInstance::getAllCommandsManager() const
{
    return *(this->allCommandsManager);
}

const CBDOptionsManager& CBDInstance::getOptionsManager() const
{
    return *(this->optionsManager);
}

const helpers::time::Timer& CBDInstance::getTimer() const
{
    return this->timer;
}

bool CBDInstance::bestEffortByTimeoutModeEnabled() const
{
    return this->bestEffortByTimeoutMode;
}

const std::string& CBDInstance::getInitialSeedsPath() const
{
    return this->initialSeedsPath.value();
}

const std::string& CBDInstance::getGotWildcardsPath() const
{
    return this->gotWildcardsPath.value();
}

int CBDInstance::getIterationNumber() const
{
    return this->iterationNumber.value();
}

bool CBDInstance::getIterativeModeStrategiesBalancingMode() const
{
    return this->iterativeModeStrategiesBalancingMode.value();
}

#pragma endregion

CBDInstance::~CBDInstance()
{
    delete this->allCommandsManager;
}

#pragma endregion

} // namespace kernel
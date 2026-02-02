#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>
#include <cstring>

#include "helpers/display/terminal.hpp"
#include "helpers/string.hpp"
#include "helpers/time/timeout_watcher.hpp"
#include "kernel/cbd_instance.hpp"
#include "kernel/cbd_state.hpp"
#include "stage/cbd_stage.hpp"
#include "kernel/cbd_options_manager.hpp"
#include "stage/cbd_stage_interface.hpp"
#include "oracle/cbd_oracle.hpp"
#include "kernel/constants.hpp"

stage::CBDStageInterface* globalCurrentStage;
kernel::CBDState internalState;
bool finishCBDRun;
std::optional<helpers::time::TimeoutWatcher> timeoutWatcher;

void usage()
{
    std::cout << helpers::display::Colors::RED_BOLD << "Usage : ./cbd [OPTIONS] <csl_file> <binary_to_fuzz_RADICAL> <time_to_fuzz_a_wildcard_in_s> <max_command_sequences_depth> <number_of_concurrent_threads> [binary_to_fuzz_args...]" << helpers::display::Colors::NEUTRAL << std::endl;
    std::cout << helpers::display::Colors::YELLOW << "Options :" << helpers::display::Colors::NEUTRAL << std::endl;
    std::cout << kernel::CBDOptionsManager::description() << std::endl;
}

void signalHandler(int signalNumber)
{
    oracle::CBDOracle* oracle;

    if (signalNumber == SIGTERM)
    {
        if ((internalState == kernel::CBDState::INPUT_GENERATION) && (globalCurrentStage->isFinishing() == false))
        {
            globalCurrentStage->stopStage();

            internalState = kernel::CBDState::IDLE;
        }

        if (internalState != kernel::CBDState::ORACLE)
        {
            std::cout << helpers::display::Colors::MAGENTA_BOLD << "Trapped SIGTERM ! Running oracle for the stage " << globalCurrentStage->getStageNumber() << helpers::display::Colors::NEUTRAL << std::endl;

            oracle = new oracle::CBDOracle(*globalCurrentStage, globalCurrentStage->getGeneratedSequencesPath(), "generated_" + std::to_string(globalCurrentStage->getStageNumber()));

            internalState = kernel::CBDState::ORACLE;

            oracle->runOracle();

            internalState = kernel::CBDState::IDLE;

            exit(0);
        }
        else
        {
            std::cout << helpers::display::Colors::MAGENTA_BOLD << "Trapped SIGTERM ! Continuing oracle execution for the stage " << globalCurrentStage->getStageNumber() << helpers::display::Colors::NEUTRAL << std::endl;

            finishCBDRun = true;
        }
    }

    if (timeoutWatcher.has_value() == true)
    {
        timeoutWatcher.value().wait();
    }
}

void printGlobalCBDMessage(std::string message)
{
    std::cout << helpers::display::Colors::GREEN_BOLD << "[CBD]" << helpers::display::Colors::NEUTRAL << ' ' << message << std::endl;
}

void printTimeTookByOracles(std::time_t totalTimeTookByCBD, float fuzzingTime)
{
    int timeTookByOracles = totalTimeTookByCBD - fuzzingTime;

    printGlobalCBDMessage("Total time took by oracles : " + std::to_string(timeTookByOracles) + " seconds");
}

int main(int argc, char** argv)
{
    std::string cslFilePath;
    std::string binaryToFuzzRadicalPath;
    std::vector<std::string> binaryToFuzzArgs;
    std::string timeToFuzzAsSting;
    int result = 0;
    int i;
    std::string maxCommandSequencesDepthAsString;
    int maxCommandSequencesDepth;
    std::string numberOfConcurrentThreadsAsString;
    int numberOfConcurrentThreads;
    const long numberOfCPUOnSystem = sysconf(_SC_NPROCESSORS_ONLN);
    int argvShift = 0;
    bool supposeThatAFLSystemOptimizationsAreAlreadyEnabled;
    kernel::CBDOptionsManager* optionsManager = nullptr;
    oracle::CBDOracle* oracle;
    bool customFuzzingTimePerFuzzingSession;
    std::vector<float> timeToFuzz;
    std::vector<std::string> timeToFuzzAsStingSplit;
    float timeToFuzzFloat;
    std::size_t j;
    bool executeOracleAfterEachStage;
    bool bestEffortByTimeoutModeEnabled;
    struct sigaction sigactionSIGTERM;
    std::optional<int> bestEffortByTimeoutModeTimeout = std::nullopt;
    std::string initialSeedsPath;
    std::string gotWildcardsPath;
    std::string iterationNumberAsString;
    int iterationNumber;
    float totalTimeTookByFuzzingSessions;
    std::optional<bool> iterativeModeStrategiesBalancingMode = std::nullopt;
    std::optional<std::thread> oracleThread = std::nullopt;
    char* alreadyProcessedByOracleSHA256SumsOfConcreteInputsRawFilePath;
    std::optional<std::string> alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath = std::nullopt;

    internalState = kernel::CBDState::PREPARATION;
    finishCBDRun = false;

    #ifdef DEBUG

    std::cout << "[CBD] WARNING : Debug version of CBD" << std::endl;

    #endif

    if (argc < 6)
    {
        usage();
    }
    else
    {
        if (argv[1][0] == '-')
        {
            optionsManager = new kernel::CBDOptionsManager(argv[1]);
            argvShift = 1;
        }
        else
        {
            optionsManager = new kernel::CBDOptionsManager();
        }

        cslFilePath = std::string(argv[1 + argvShift]);
        binaryToFuzzRadicalPath = std::string(argv[2 + argvShift]);
        timeToFuzzAsSting = std::string(argv[3 + argvShift]);
        maxCommandSequencesDepthAsString = std::string(argv[4 + argvShift]);
        numberOfConcurrentThreadsAsString = std::string(argv[5 + argvShift]);

        supposeThatAFLSystemOptimizationsAreAlreadyEnabled = optionsManager->isDefined(kernel::CBDOptionsManager::Options::SUPPOSED_THAT_AFL_SYSTEM_OPTIMIZATIONS_ARE_ALREADY_ENABLED);
        customFuzzingTimePerFuzzingSession = optionsManager->isDefined(kernel::CBDOptionsManager::Options::CUSTOM_FUZZING_TIME_PER_FUZZING_SESSION);
        executeOracleAfterEachStage = optionsManager->isDefined(kernel::CBDOptionsManager::Options::ORACLE_TESTING_AFTER_EACH_STAGE);
        bestEffortByTimeoutModeEnabled = optionsManager->isDefined(kernel::CBDOptionsManager::Options::BEST_EFFORT_BY_TIMEOUT);

        if (optionsManager->isDefined(kernel::CBDOptionsManager::Options::ITERATIVE_MODE) == true)
        {
            iterativeModeStrategiesBalancingMode = optionsManager->isDefined(kernel::CBDOptionsManager::Options::ITERATIVE_MODE_STRATEGIES_BALANCING);

            initialSeedsPath = std::string(argv[6 + argvShift]);
            gotWildcardsPath = std::string(argv[7 + argvShift]);
            iterationNumberAsString = std::string(argv[8 + argvShift]);

            try
            {
                iterationNumber = std::stoi(iterationNumberAsString);
            }
            catch (const std::invalid_argument& e)
            {
                std::cout << "ERROR : <iteration_number> argument is invalid (Integer expected, provided \"" << iterationNumberAsString << "\")" << std::endl;

                result = 1;
            }

            if (optionsManager->isDefined(kernel::CBDOptionsManager::Options::REMOVE_ALTREADY_TREATED_CONCRETE_INPUTS_BEFORE_ORACLE))
            {
                alreadyProcessedByOracleSHA256SumsOfConcreteInputsRawFilePath = std::getenv(ALREADY_TREATED_CONCRETE_INPUTS_SHA256_SUMS_PATH_ENV);

                if (alreadyProcessedByOracleSHA256SumsOfConcreteInputsRawFilePath != NULL)
                {
                    alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath = std::string(alreadyProcessedByOracleSHA256SumsOfConcreteInputsRawFilePath);
                }
            }

            for (i = 9 + argvShift; i < argc; ++i)
            {
                binaryToFuzzArgs.push_back(argv[i]);
            }
        }
        else
        {
            for (i = 6 + argvShift; i < argc; ++i)
            {
                binaryToFuzzArgs.push_back(argv[i]);
            }
        }

        try
        {
            maxCommandSequencesDepth = std::stoi(maxCommandSequencesDepthAsString);
        }
        catch(const std::exception& e)
        {
            std::cout << helpers::display::Colors::RED_BOLD << "ERROR : <max_command_sequences_depth> argument is invalid" << helpers::display::Colors::NEUTRAL << std::endl;

            result = 1;
        }

        memset(&sigactionSIGTERM, 0, sizeof(struct sigaction));

        sigactionSIGTERM.sa_handler = signalHandler;

        sigaction(SIGTERM, &sigactionSIGTERM, NULL);

        if (bestEffortByTimeoutModeEnabled == true)
        {
            bestEffortByTimeoutModeTimeout = maxCommandSequencesDepth;

            timeoutWatcher = helpers::time::TimeoutWatcher((std::time_t)bestEffortByTimeoutModeTimeout.value()); // maxCommandSequencesDepth stands now for maximal timeout (See usage())
            maxCommandSequencesDepth = 2; // We begin with a depth of 2, because the adversarial sequence generation for the next stage needs to have adversarial sequences of depth n + 1

            timeoutWatcher->run();
        }

        if ((result == 0) && (customFuzzingTimePerFuzzingSession == true))
        {
            timeToFuzzAsStingSplit = helpers::string::split(timeToFuzzAsSting, ',');
            j = 0;

            if ((bestEffortByTimeoutModeEnabled == false) && (timeToFuzzAsStingSplit.size() != (std::size_t)maxCommandSequencesDepth))
            {
                std::cout << helpers::display::Colors::RED_BOLD << "ERROR : The depth of the command sequences is not the same as the number of provided fuzzing times" << helpers::display::Colors::NEUTRAL << std::endl;
            }

            totalTimeTookByFuzzingSessions = 0;

            while ((result == 0) && (j < timeToFuzzAsStingSplit.size()))
            {
                try
                {
                    timeToFuzz.push_back(std::stof(timeToFuzzAsStingSplit.at(j)));

                    totalTimeTookByFuzzingSessions += std::stof(timeToFuzzAsStingSplit.at(j));

                    ++j;
                }
                catch (const std::invalid_argument& e)
                {
                    std::cout << "ERROR : <time_to_fuzz_in_s> argument is invalid (Syntax : \"<int>,<int> ...\")" << std::endl;

                    result = 1;
                }
            }
        }
        else
        {
            try
            {
                timeToFuzzFloat = std::stof(timeToFuzzAsSting);
            }
            catch(const std::invalid_argument& e)
            {
                std::cout << "ERROR : <time_to_fuzz_in_s> argument is invalid (Type : Float)" << std::endl;

                result = 1;
            }

            totalTimeTookByFuzzingSessions = timeToFuzzFloat * maxCommandSequencesDepth;

            for (i = 0; i < maxCommandSequencesDepth; ++i)
            {
                timeToFuzz.push_back(timeToFuzzFloat);
            }
        }

        try
        {
            numberOfConcurrentThreads = std::stoi(numberOfConcurrentThreadsAsString);
        }
        catch(const std::exception& e)
        {
            std::cout << "ERROR : <number_of_concurrent_fuzzing_threads> argument is invalid" << std::endl;

            result = 1;
        }

        if ((result == 0) && (numberOfConcurrentThreads > numberOfCPUOnSystem))
        {
            std::cout << "ERROR : <number_of_concurrent_fuzzing_threads> argument is invalid because it is upper than the number of CPUs on this system (" << numberOfCPUOnSystem << ")" << std::endl;

            result = 1;
        }

        if (result == 0)
        {
            printGlobalCBDMessage("Initialization and generation of CSL related utils");

            if (optionsManager->isDefined(kernel::CBDOptionsManager::Options::ITERATIVE_MODE) == true)
            {
                kernel::CBDInstance::initializeInstance(
                    cslFilePath,
                    binaryToFuzzRadicalPath,
                    timeToFuzz,
                    maxCommandSequencesDepth,
                    "./input",
                    binaryToFuzzArgs,
                    numberOfConcurrentThreads,
                    optionsManager,
                    initialSeedsPath,
                    gotWildcardsPath,
                    iterationNumber,
                    iterativeModeStrategiesBalancingMode.value()
                );
            }
            else
            {
                kernel::CBDInstance::initializeInstance(cslFilePath, binaryToFuzzRadicalPath, timeToFuzz, maxCommandSequencesDepth, "./input", binaryToFuzzArgs, numberOfConcurrentThreads, optionsManager);
            }

            if (supposeThatAFLSystemOptimizationsAreAlreadyEnabled == false)
            {
                printGlobalCBDMessage("Enabling system optimizations for AFL++");

                kernel::CBDInstance::getInstance().enableAFLSystemOptimizations();
            }

            if (finishCBDRun == false)
            {
                globalCurrentStage = new stage::CBDStage(kernel::CBDInstance::getInstance(), 0);

                internalState = kernel::CBDState::INPUT_GENERATION;

                globalCurrentStage->runStage();

                internalState = kernel::CBDState::IDLE;
            }

            i = 1;

            while (((bestEffortByTimeoutModeEnabled == true) && (finishCBDRun == false)) || ((i < maxCommandSequencesDepth) && (finishCBDRun == false)))
            {
                if ((executeOracleAfterEachStage == true) && ((i < maxCommandSequencesDepth) || (bestEffortByTimeoutModeEnabled == true)))
                {
                    oracle = new oracle::CBDOracle(*globalCurrentStage, globalCurrentStage->getGeneratedSequencesPath(), "generated_" + std::to_string(globalCurrentStage->getStageNumber()), alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath);

                    internalState = kernel::CBDState::ORACLE;

                    oracle->runOracle();

                    internalState = kernel::CBDState::IDLE;

                    if (finishCBDRun == false)
                    {
                        globalCurrentStage = new stage::CBDStage(*((stage::CBDStage*)globalCurrentStage), i, oracle->getRelatedAdversarialSequencesIdsInRelatedStageThatCorrespondToFoundBackdoors());
                    }

                    delete oracle;
                }
                else if (finishCBDRun == false)
                {
                    globalCurrentStage = new stage::CBDStage(*((stage::CBDStage*)globalCurrentStage), i);

                    if ((i == (maxCommandSequencesDepth - 1)) && (optionsManager->isDefined(kernel::CBDOptionsManager::Options::ORACLE_PARALLELIZATION_BY_EXECUTING_STATIC_SEQUENCES_ORACLE_DURING_LAST_FUZZING_STAGE) == true))
                    {
                        oracle = new oracle::CBDOracle(*globalCurrentStage, globalCurrentStage->getStaticSequencesPath(), "static_" + std::to_string(globalCurrentStage->getStageNumber()), alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath);

                        oracleThread = std::thread([oracle] () {
                            oracle->runOracle();

                            delete oracle;
                        });
                    }
                }

                if ((bestEffortByTimeoutModeEnabled == true) && (finishCBDRun == false))
                {
                    printGlobalCBDMessage("Running cslprocess to manage deeper command sequences");

                    kernel::CBDInstance::getInstance().increaseMaxSequenceDepthAndRerunCSLProcess();
                }

                if (finishCBDRun == false)
                {
                    internalState = kernel::CBDState::INPUT_GENERATION;

                    globalCurrentStage->runStage();

                    internalState = kernel::CBDState::IDLE;
                }

                ++i;
            }

            if (finishCBDRun == false)
            {
                oracle = new oracle::CBDOracle(*globalCurrentStage, globalCurrentStage->getGeneratedSequencesPath(), "generated_" + std::to_string(globalCurrentStage->getStageNumber()), alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath);

                internalState = kernel::CBDState::ORACLE;

                oracle->runOracle();

                delete oracle;

                if (optionsManager->isDefined(kernel::CBDOptionsManager::Options::ORACLE_PARALLELIZATION_BY_EXECUTING_STATIC_SEQUENCES_ORACLE_DURING_LAST_FUZZING_STAGE) == false)
                {
                    oracle = new oracle::CBDOracle(*globalCurrentStage, globalCurrentStage->getStaticSequencesPath(), "static_" + std::to_string(globalCurrentStage->getStageNumber()), alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath);

                    oracle->runOracle();

                    internalState = kernel::CBDState::IDLE;

                    delete oracle;
                }
            }

            if (oracleThread != std::nullopt)
            {
                oracleThread->join();
            }

            printTimeTookByOracles(kernel::CBDInstance::getInstance().getTimer().getNumberOfElapsedSeconds(), totalTimeTookByFuzzingSessions);

            printGlobalCBDMessage("This is the end !");
        }
    }

    return result;
}
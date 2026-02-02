#include <iostream>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <filesystem>
#include <utility>
#include <vector>

#include "oracle/cbd_oracle.hpp"
#include "helpers/display/terminal.hpp"
#include "helpers/filesystem.hpp"
#include "helpers/runnables/bash_script_execution.hpp"
#include "helpers/runnables/job.hpp"
#include "helpers/runnables/jobs_manager.hpp"
#include "kernel/cbd_instance.hpp"
#include "kernel/constants.hpp"
#include "stage/cbd_stage_interface.hpp"
#include "oracle/internals/cbd_oracle_csl_automaton_run.hpp"
#include "oracle/internals/cbd_oracle_run_syscall_monitor.hpp"
#include "helpers/runnables/runnable.hpp"
#include "oracle/internals/cbd_oracle_build_syscalls.hpp"

namespace oracle
{

#pragma region CBDOracleUnableToCreateDirectoryException

CBDOracleUnableToCreateDirectoryException::CBDOracleUnableToCreateDirectoryException(std::string _directory) :
    message("ERROR : Unable to create the directory \"" + _directory + "\"\n")
{
}

const char* CBDOracleUnableToCreateDirectoryException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion
#pragma region CBDOracleNotAlreadyExecutedException

const char* CBDOracleNotAlreadyExecutedException::what() const throw()
{
    return "ERROR : The oracle was not already executed";
}

#pragma endregion

void CBDOracle::printOracleMessage(std::string message)
{
    std::cout << helpers::display::Colors::BLUE_BOLD << "[CBD oracle "<< this->tag << "]" << helpers::display::Colors::NEUTRAL << ' ' << message << std::endl;
}

void CBDOracle::buildSyscalls()
{
    internals::CBDOracleBuildSyscalls* oracleBuildSyscallsModule;

    oracleBuildSyscallsModule = new internals::CBDOracleBuildSyscalls(*this, this->relatedStage, this->inputsDir);

    oracleBuildSyscallsModule->run();
    oracleBuildSyscallsModule->wait();

    delete oracleBuildSyscallsModule;

    this->builtSyscalls = true;
}

void CBDOracle::runSyscallMonitor()
{
    std::string outputDirSyscallMonitorForThisStage;
    internals::CBDOracleRunSyscallMonitor* oracleRunSyscallMonitorModule;

    outputDirSyscallMonitorForThisStage = OUTPUT_DIR_SYSCALL_MONITOR "/" + this->tag;

    std::filesystem::create_directory(outputDirSyscallMonitorForThisStage);

    this->relatedSyscallMonitorOutputDir = outputDirSyscallMonitorForThisStage;

    if (this->builtSyscalls == true)
    {
        oracleRunSyscallMonitorModule = new internals::CBDOracleRunSyscallMonitor(*this, this->inputsDir);

        oracleRunSyscallMonitorModule->run();
        oracleRunSyscallMonitorModule->wait();

        delete oracleRunSyscallMonitorModule;
    }
}

void CBDOracle::runCSLAutomaton()
{
    std::string currentSyscallMonitorResultPath;
    internals::CBDOracleCSLAutomatonRun* automatonRun;
    std::vector<helpers::runnables::Job*> automatonRuns;
    std::optional<helpers::runnables::JobsManager> automatonRunsManager;
    std::optional<std::map<const helpers::runnables::Job*, int>> automatonResultsCode;
    std::filesystem::path currentBackdoorPath;

    currentSyscallMonitorResultPath = this->relatedSyscallMonitorOutputDir;

    for (const std::filesystem::directory_entry& currentResult : std::filesystem::directory_iterator(currentSyscallMonitorResultPath))
    {
        if (currentResult.is_regular_file() == true)
        {
            automatonRun = new internals::CBDOracleCSLAutomatonRun(currentResult.path(), this->inputsDir);

            automatonRuns.push_back(automatonRun);
        }
    }

    automatonRunsManager = std::optional<helpers::runnables::JobsManager>(helpers::runnables::JobsManager(automatonRuns, this->relatedStage.getRelatedInstance().getNumberOfConcurrentThreads()));

    automatonRunsManager->runRunnables();

    automatonResultsCode = automatonRunsManager->getResultsCode();

    if (automatonResultsCode.has_value() == true)
    {
        for (std::pair<const helpers::runnables::Job*, int> pair : automatonResultsCode.value())
        {
            if (pair.second != 0)
            {
                automatonRun = (internals::CBDOracleCSLAutomatonRun*)pair.first;
                currentBackdoorPath = automatonRun->getAssociatedCommandsFilePath();

                std::filesystem::copy_file(currentBackdoorPath, std::filesystem::path(this->potentialBackdoorsPath + "/" + currentBackdoorPath.filename().string()));
            }
        }
    }

    for (helpers::runnables::Runnable* currentAutomatonRun : automatonRuns)
    {
        delete currentAutomatonRun;
    }

    automatonRuns.clear();
}

void CBDOracle::findStrangeCommandSequences()
{
    std::string currentSyscallMonitorResultPath;

    currentSyscallMonitorResultPath = this->relatedSyscallMonitorOutputDir + "/strange";

    for (const std::filesystem::directory_entry& currentStrangeCommandSequencePath : std::filesystem::directory_iterator(currentSyscallMonitorResultPath))
    {
        if (currentStrangeCommandSequencePath.path().filename() == "__report__.txt")
        {
            std::filesystem::copy_file(currentStrangeCommandSequencePath, std::filesystem::path(this->potentialBackdoorsPath + "/" + currentStrangeCommandSequencePath.path().filename().string()));
        }
        else
        {
            std::filesystem::copy_file(currentStrangeCommandSequencePath, std::filesystem::path(this->potentialBackdoorsPath + "/" + currentStrangeCommandSequencePath.path().filename().string() + "_strange"));
        }
    }
}

void CBDOracle::removeAlreadyProcessedConcreteInputs() const
{
    std::string scriptArgs = this->alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath.value() + " " + std::filesystem::absolute(this->inputsDir).string();
    helpers::runnables::BashScriptExecution bse("/usr/local/lib/cbd/reduce_generated_inputs_for_oracle.sh", scriptArgs);

    bse.run();
    bse.wait();
}

CBDOracle::CBDOracle(const stage::CBDStageInterface& _relatedStage, std::filesystem::path _inputsDir, std::string _tag) :
    relatedStage(_relatedStage),
    inputsDir(_inputsDir),
    tag(_tag),
    potentialBackdoorsPath(OUTPUT_DIR_POTENTIAL_BACKDOORS "/" + this->tag)
{
    std::filesystem::create_directories(this->potentialBackdoorsPath);
}

CBDOracle::CBDOracle(const stage::CBDStageInterface& _relatedStage, std::filesystem::path _inputsDir, std::string _tag, std::optional<std::string> _alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath) :
    relatedStage(_relatedStage),
    inputsDir(_inputsDir),
    tag(_tag),
    potentialBackdoorsPath(OUTPUT_DIR_POTENTIAL_BACKDOORS "/" + this->tag),
    alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath(_alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath)
{
    std::filesystem::create_directories(this->potentialBackdoorsPath);
}

void CBDOracle::runOracle()
{
    if (this->alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath != std::nullopt)
    {
        this->printOracleMessage("Removing already processed concrete inputs");
        this->removeAlreadyProcessedConcreteInputs();
    }

    this->printOracleMessage("Getting interesting syscalls through the generated script from the CSL specification");
    this->buildSyscalls();

    this->printOracleMessage("Using syscall_monitor to keep only accepted commands");
    this->runSyscallMonitor();

    this->printOracleMessage("Keeping suspected command sequences with the CSL's automaton");
    this->runCSLAutomaton();

    this->printOracleMessage("Finding strange command sequences that relate to strange program behavior");
    this->findStrangeCommandSequences();

    this->printOracleMessage("Printing found backdoors :");

    for (const std::string& backdoor : this->getFoundBackdoors())
    {
        std::cout << backdoor << "-------------" << std::endl;
    }

    this->printOracleMessage("Elapsed seconds after the beginning of the CBD run : " + std::to_string(this->relatedStage.getRelatedInstance().getTimer().getNumberOfElapsedSeconds()));
}

std::vector<std::string> CBDOracle::getFoundBackdoors() const
{
    std::vector<std::string> result;
    std::string currentBackdoorFileContent;

    for (const std::filesystem::directory_entry& backdoor : std::filesystem::directory_iterator(this->potentialBackdoorsPath))
    {
        if (backdoor.path().filename() != "__report__.txt")
        {
            currentBackdoorFileContent = helpers::filesystem::getFileContent(backdoor.path().string());

            result.push_back(currentBackdoorFileContent);
        }
    }

    return result;
}

std::string CBDOracle::getTag() const
{
    return this->tag;
}

const stage::CBDStageInterface& CBDOracle::getRelatedStage() const
{
    return this->relatedStage;
}

const std::vector<int>& CBDOracle::getRelatedAdversarialSequencesIdsInRelatedStageThatCorrespondToFoundBackdoors() const
{
    return this->relatedAdversarialSequencesIdsInRelatedStageThatCorrespondToFoundBackdoors;
}

} // namespace oracle